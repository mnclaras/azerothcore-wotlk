-- DB: characters

CREATE TABLE IF NOT EXISTS `template_npc_alliance` (
  `playerClass` varchar(50) NOT NULL,
  `playerSpec` varchar(50) NOT NULL,
  `pos` int(10) unsigned NOT NULL DEFAULT '0',
  `itemEntry` int(10) unsigned NOT NULL DEFAULT '0',
  `enchant` int(10) unsigned NOT NULL DEFAULT '0',
  `socket1` int(10) unsigned NOT NULL DEFAULT '0',
  `socket2` int(10) unsigned NOT NULL DEFAULT '0',
  `socket3` int(10) unsigned NOT NULL DEFAULT '0',
  `bonusEnchant` int(10) unsigned NOT NULL DEFAULT '0',
  `prismaticEnchant` int(10) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Templates';


CREATE TABLE IF NOT EXISTS `template_npc_horde` (
  `playerClass` varchar(50) NOT NULL,
  `playerSpec` varchar(50) NOT NULL,
  `pos` int(10) unsigned NOT NULL DEFAULT '0',
  `itemEntry` int(10) unsigned NOT NULL DEFAULT '0',
  `enchant` int(10) unsigned NOT NULL DEFAULT '0',
  `socket1` int(10) unsigned NOT NULL DEFAULT '0',
  `socket2` int(10) unsigned NOT NULL DEFAULT '0',
  `socket3` int(10) unsigned NOT NULL DEFAULT '0',
  `bonusEnchant` int(10) unsigned NOT NULL DEFAULT '0',
  `prismaticEnchant` int(10) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Templates';


CREATE TABLE IF NOT EXISTS `template_npc_human` (
  `playerClass` varchar(50) NOT NULL,
  `playerSpec` varchar(50) NOT NULL,
  `pos` int(10) unsigned NOT NULL DEFAULT '0',
  `itemEntry` int(10) unsigned NOT NULL DEFAULT '0',
  `enchant` int(10) unsigned NOT NULL DEFAULT '0',
  `socket1` int(10) unsigned NOT NULL DEFAULT '0',
  `socket2` int(10) unsigned NOT NULL DEFAULT '0',
  `socket3` int(10) unsigned NOT NULL DEFAULT '0',
  `bonusEnchant` int(10) unsigned NOT NULL DEFAULT '0',
  `prismaticEnchant` int(10) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Templates';


CREATE TABLE IF NOT EXISTS `template_npc_talents` (
  `playerClass` varchar(50) NOT NULL,
  `playerSpec` varchar(50) NOT NULL,
  `talentId` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Templates';


CREATE TABLE IF NOT EXISTS `template_npc_glyphs` (
  `playerClass` varchar(50) NOT NULL,
  `playerSpec` varchar(50) NOT NULL,
  `slot` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `glyph` smallint(5) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Templates';

