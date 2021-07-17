#include "ScriptMgr.h"
#include "Chat.h"
#include <list>
#include <algorithm>
#include <cctype>
#include <string>
#include "Log.h"
#include "Define.h"
#include "Player.h"
#include "DBCStores.h"
#include "World.h"
#include "Configuration/Config.h"


std::vector<std::string>chat;

class SystemCensure : public PlayerScript
{
public:
    SystemCensure() : PlayerScript("SystemCensure") {}

    void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg) override
    {
        CheckMessage(player, msg, lang, NULL, NULL, NULL, NULL);
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Player* receiver) override
    {
        CheckMessage(player, msg, lang, receiver, NULL, NULL, NULL);
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Group* group) override
    {
        CheckMessage(player, msg, lang, NULL, group, NULL, NULL);
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Guild* guild) override
    {
        CheckMessage(player, msg, lang, NULL, NULL, guild, NULL);
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel) override
    {
        CheckMessage(player, msg, lang, NULL, NULL, NULL, channel);
    }

    void CheckMessage(Player* player, std::string& msg, uint32 /*lang*/, Player* /*receiver*/, Group* /*group*/, Guild* /*guild*/, Channel* /*channel*/)
    {
        //if account is game master let them say what ever they like just incase they need to send the website
        //if (player->GetSession()->GetSecurity() >= 1)
        //    return;

        std::string CheckMsg = msg;
        std::string FullMessage = msg;

        try
        {
            if (IsBadMessage(CheckMsg))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Los links y la publicidad no son bienvenidos!");
                SendGMTexts(player, CheckMsg, FullMessage);
                msg = "";
            }
        }
        catch (std::exception& e)
        {
        }
    }

    void SendGMTexts(Player* player, std::string /*ADMessage*/, std::string FullMessage)
    {
        // display warning at the center of the screen, hacky way?
        std::string str = "";
        str = "|cFFFFFC00[Player]:|cFF00FFFF[|cFF60FF00" + std::string(player->GetName().c_str()) + "|cFF00FFFF] mensaje prohibido: [" + std::string(FullMessage.c_str()) + "]";

        uint32 accountId = player->GetSession()->GetAccountId();
        if (accountId && accountId > 0)
        {
            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAT_CENSURE_LOG);
            stmt->setUInt32(0, accountId);
            CharacterDatabase.EscapeString(FullMessage);
            stmt->setString(1, FullMessage);
            CharacterDatabase.Execute(stmt);
        }

        WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
        data << str;
        sWorld->SendGlobalGMMessage(&data);
    }

    bool IsBadMessage(std::string& msg)
    {
        // transform to lowercase (for simpler checking)
        std::string lower = msg;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        // Now we converted to lower lets remove the spaces
        lower.erase(std::remove_if(lower.begin(), lower.end(), ::isspace), lower.end());

        for (auto const& itr : chat)
        {
            if (lower.find(itr) != std::string::npos)
            {
                msg = itr;
                return true;
            }
        }

        return false;
    }
};

class LoadChatTable : public WorldScript
{
public:
    LoadChatTable() : WorldScript("load_system_censure") { }

	void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/antiad.conf";
            std::string cfg_def_file = cfg_file + ".dist";

            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }

    void OnLoadCustomDatabaseTable() override
    {
        sLog->outString("Loading Chat Censure...");

        QueryResult result = CharacterDatabase.PQuery("SELECT `id`,`text` FROM chat_censure");

        if (!result)
        {
            sLog->outErrorDb(">>  Loaded 0 Chat Censures. DB table `Chat_Censure` is empty!");
            sLog->outString();
            return;
        }

        uint32 count = 0;
        uint32 oldMSTime = getMSTime();

        do
        {
            Field* field = result->Fetch();
            //uint8 id = field[0].GetUInt8();
            chat.push_back(field[1].GetString());

            count++;

        } while (result->NextRow());

        sLog->outString(">> Loaded %u chat_censure in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        sLog->outString("");
    }
};

class ChatCensureCommand : public CommandScript
{
public:
    ChatCensureCommand() : CommandScript("ChatCensureCommand") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> ChatCensureCommandTable =
        {
            { "Reload", SEC_ADMINISTRATOR, true, &HandleReloadCommand, "Realod the chat Censure table" },
            { "Add",    SEC_ADMINISTRATOR, false, &HandleAddCommand,    "Ban a word. Please use quotation marks when adding."},
            { "Delete", SEC_ADMINISTRATOR, false, &HandleDeleteCommand, "Delete a banned word. Please use quotation marks when deleting"}
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "ChatCensure", SEC_ADMINISTRATOR, false, nullptr, "", ChatCensureCommandTable }
        };

        return commandTable;
    }

    static bool HandleReloadCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* me = handler->GetSession()->GetPlayer();

        if (!me)
            return false;

        chat.clear();
        QueryResult result = CharacterDatabase.PQuery("SELECT `id`,`text` FROM chat_censure");
        uint32 count = 0;
        uint32 oldMSTime = getMSTime();

        do
        {
            Field* field = result->Fetch();
            //uint8 id = field[0].GetUInt8();
            chat.push_back(field[1].GetString());

            count++;

        } while (result->NextRow());

        ChatHandler(me->GetSession()).PSendSysMessage("Reloaded %u chat censure in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        return true;

    }

    static bool HandleAddCommand(ChatHandler* handler, char const* args)
    {
        Player* me = handler->GetSession()->GetPlayer();

        if (!me)
            return false;

        if (!args)
            return false;

        char* textExtracted = handler->extractQuotedArg((char*)args);

        if (!textExtracted)
            return false;

        std::string text = textExtracted;

        //lets check the Database to see if arguement already exist
        QueryResult result = CharacterDatabase.PQuery("SELECT `text` FROM `chat_censure` WHERE `text` = '%s'", text.c_str());

        if (result)
        {
            ChatHandler(me->GetSession()).PSendSysMessage("Duplicate text entry for text: |cff4CFF00 %s|r", text.c_str());
        }
        else
        {
            CharacterDatabase.PQuery("INSERT INTO `chat_censure` (`text`) VALUES ('%s')", text.c_str());
            ChatHandler(me->GetSession()).PSendSysMessage("Added the text: |cff4CFF00 %s|r to chat censure", text.c_str());
        }
        return true;
    }

    static bool HandleDeleteCommand(ChatHandler* handler, char const* args)
    {
        Player* me = handler->GetSession()->GetPlayer();

        if (!me)
            return false;

        if (!args)
            return false;

        //we want argument to be in "" for spaces ect.
        char* textExtracted = handler->extractQuotedArg((char*)args);

        if (!textExtracted)
            return false;

        std::string text = textExtracted;

        QueryResult result = CharacterDatabase.PQuery("SELECT `text` FROM `chat_censure` WHERE `text` = '%s'", text.c_str());

        if (!result)
        {
            ChatHandler(me->GetSession()).PSendSysMessage("Could not find text: |cff4CFF00 %s|r in the Database", text.c_str());
        }
        else
        {
            CharacterDatabase.PQuery("DELETE FROM `chat_censure` WHERE `text` = '%s'", text.c_str());
            ChatHandler(me->GetSession()).PSendSysMessage("Deleted Text |cff4CFF00 %s|r please reload the table", text.c_str());
        }

        return true;
    }

};


void AddSC_AntiAD()
{
	new ChatCensureCommand();
    new SystemCensure();
    new LoadChatTable();
}
