/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE TABLE IF NOT EXISTS `guild_points_ranking` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `guildId` int(10) unsigned NOT NULL DEFAULT '0',
  `seasonPoints` int(10) unsigned NOT NULL DEFAULT '0',
  `guildHousePoints` int(10) unsigned NOT NULL DEFAULT '0',
  `totalPoints` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `unique_guild` (`guildId`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `guild_points_boss_reward` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `points` int(10) unsigned NOT NULL DEFAULT '0',
  `mode` int(10) unsigned NOT NULL DEFAULT '0',
  `difficulty` varchar(1) DEFAULT 'X',
  PRIMARY KEY (`id`),
  UNIQUE KEY `unique_reward` (`entry`,`mode`,`difficulty`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;


/* ALTER guild_points_ranking */
ALTER TABLE guild_points_ranking CHANGE points seasonPoints INT(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE guild_points_ranking ADD guildHousePoints INT(10) unsigned NOT NULL DEFAULT '0' AFTER seasonPoints;
ALTER TABLE guild_points_ranking ADD totalPoints INT(10) unsigned NOT NULL DEFAULT '0' AFTER guildHousePoints;
UPDATE guild_points_ranking SET guildHousePoints = seasonPoints, totalPoints = seasonPoints;
/* ALTER guild_points_boss_reward */
ALTER TABLE `guild_points_boss_reward` ADD UNIQUE `unique_reward`(`entry`, `mode`, `difficulty`);
ALTER TABLE `guild_points_ranking` ADD UNIQUE `unique_guild`(`guildId`);


-- Dumping structure for table acore_char.guild_house
CREATE TABLE IF NOT EXISTS `guild_house` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `guild` int(10) NOT NULL DEFAULT '0',
  `phase` int(10) NOT NULL DEFAULT '0',
  `guild_position` int(10) NOT NULL DEFAULT '0',
  `map` int(10) unsigned NOT NULL DEFAULT '0',
  `zone` int(10) unsigned NOT NULL DEFAULT '0',
  `area` int(10) unsigned NOT NULL DEFAULT '0',
  `posX` float NOT NULL DEFAULT '0',
  `posY` float NOT NULL DEFAULT '0',
  `posZ` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `guild` (`guild`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `guild_house_position` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL DEFAULT '',
  `map` int(10) unsigned NOT NULL DEFAULT '0',
  `zone` int(10) unsigned NOT NULL DEFAULT '0',
  `area` int(10) unsigned NOT NULL DEFAULT '0',
  `posX` float NOT NULL DEFAULT '0',
  `posY` float NOT NULL DEFAULT '0',
  `posZ` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `guild_house_purchased_spawns` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `guild` int(10) unsigned NOT NULL DEFAULT '0',
  `spawn` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `guild_points_allowed_members` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `guildId` int(10) unsigned NOT NULL DEFAULT '0',
  `member` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `unique_guild_member` (`guildId`,`member`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;


-- Data exporting was unselected.
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;