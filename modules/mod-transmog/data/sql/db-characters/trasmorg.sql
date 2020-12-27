-- Dumping structure for table tc_c.custom_transmogrification
CREATE TABLE IF NOT EXISTS `custom_transmogrification` (
  `GUID` int(10) unsigned NOT NULL COMMENT 'Item guidLow',
  `FakeEntry` int(10) unsigned NOT NULL COMMENT 'Item entry',
  `Owner` int(10) unsigned NOT NULL COMMENT 'Player guidLow',
  PRIMARY KEY (`GUID`),
  KEY `Owner` (`Owner`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='6_2';

-- Data exporting was unselected.


-- Dumping structure for table tc_c.custom_transmogrification_sets
CREATE TABLE IF NOT EXISTS `custom_transmogrification_sets` (
  `Owner` int(10) unsigned NOT NULL COMMENT 'Player guidlow',
  `PresetID` tinyint(3) unsigned NOT NULL COMMENT 'Preset identifier',
  `SetName` text COMMENT 'SetName',
  `SetData` text COMMENT 'Slot1 Entry1 Slot2 Entry2',
  PRIMARY KEY (`Owner`,`PresetID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='6_1';

DROP TABLE IF EXISTS `players_reports_status`;

CREATE TABLE `players_reports_status` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `creation_time` int(10) unsigned NOT NULL DEFAULT '0',
  `average` float NOT NULL DEFAULT '0',
  `total_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `speed_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `fly_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `jump_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `waterwalk_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `teleportplane_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `climb_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='';

DROP TABLE IF EXISTS `daily_players_reports`;
CREATE TABLE `daily_players_reports` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `creation_time` int(10) unsigned NOT NULL DEFAULT '0',
  `average` float NOT NULL DEFAULT '0',
  `total_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `speed_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `fly_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `jump_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `waterwalk_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `teleportplane_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  `climb_reports` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='';

DROP TABLE IF EXISTS `chat_censure`;
CREATE TABLE `chat_censure` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `text` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

INSERT INTO chat_censure (id, text) VALUES
('1', 'http://'),
('2', 'sytes'),
('4', 'h t t p : / /'),
('5', 'h t t p s : / /'),
('7', '.net'),
('8', '.org'),
('10', 'wow-'),
('12', 'no-ip'),
('13', 'zapto'),
('14', '.biz'),
('15', '.servegame'),
('16', '.br'),
('17', 'com.br'),
('18', '. c o m'),
('19', '. n e t'),
('20', '. o r g'),
('21', 'w w w .'),
('22', ' w o w -'),
('23', '- w o w'),
('24', 'n o - i p'),
('25', '. z a p t o'),
('26', '. b i z'),
('27', '. s e r v e g a m e'),
('28', '. b r'),
('29', 'c o m . b r'),
('30', 'h  t  t  p  :  /  /'),
('31', '.  c  o  m'),
('32', '.  n  e  t'),
('33', '.  o  r  g'),
('34', 'w  w  w  .'),
('35', ' w  o  w  -'),
('36', '-  w  o  w'),
('37', 'n  o  -  i  p'),
('38', '.  z  a  p  t  o'),
('39', '.  b  i  z'),
('40', '.  s  e  r  v  e  g  a  m  e'),
('41', '.  b  r'),
('42', 'c  o  m  .  b  r'),
('43', 'h   t   t   p   :   /   /'),
('44', '.   c   o   m'),
('45', '.   n   e   t'),
('46', '.   o   r   g'),
('47', 'w   w   w   .'),
('48', ' w   o   w   -'),
('49', '-   w   o   w'),
('50', 'n   o   -   i   p'),
('51', '.   z   a   p   t   o'),
('52', '.   b   i   z'),
('53', '.   s   e   r   v   e   g   a   m   e'),
('54', '.   b   r'),
('55', '   c   o   m   .   b   r'),
('56', 'h    t    t    p   :   /   /'),
('57', '.    c    o    m'),
('58', '.    n    e   t'),
('59', '.    o    r    g'),
('60', 'w    w    w    .'),
('61', 'w    o    w    -'),
('62', '-    w    o    w'),
('63', 'n    o    -    i    p'),
('64', '.    z    a    p    t    o'),
('65', '.    b    i     z'),
('66', '.    s    e    r    v    e    g    a    m    e'),
('67', '.    b    r'),
('68', 'c    o    m    .    b    r'),
('69', 'h  t  t  p  s  :  /  /'),
('70', 'wowmx'),
('71', 'w o w m x'),
('72', 'mx wow'),
('73', 'm x w o w'),
('74', 'imperius'),
('75', 'i m p e r i u s'),
('77', 's e r v i d o r'),
('78', 's.e.r.v.i.d.o.r'),
('79', 's-e-r-v-i-d-o-r'),
('80', 'xeneize'),
('81', 's u l v u s'),
('82', 's ulvus'),
('83', 'sulvus'),
('84', 'su lvus'),
('85', 'sul vus'),
('86', 'sulv us'),
('87', 'sulvu s'),
('88', 'su1vus'),
('89', '5u1vu5'),
('90', 'su l vus'),
('91', 's ulv us'),
('92', 's ulv u s'),
('93', 'im p e r i u s'),
('94', 'i mp e r i u s'),
('95', 'i mperius'),
('96', '1mperius'),
('97', '1mper1us'),
('98', 'imp3rius'),
('99', 'imp3riu5'),
('100', 'imper ius'),
('101', 'imperiu'),
('102', 'damos promocion'),
('103', 'regalamos t10'),
('104', 'damos t10'),
('105', 'asco de sv'),
('106', 'asco de server'),
('107', 'asco de servidor'),
('108', 's u l vus'),
('109', 'su l v us'),
('110', '5u1vus'),
('111', 'su1vu5'),
('112', 'sulvu5'),
('113', 'sulvu'),
('114', 'sulv u'),
('115', 'tournament-wow'),
('116', 'tournamentwow'),
('117', 'garona'),
('118', 'garona-wow'),
('119', 'garonawow'),
('120', 'garona wow'),
('121', 'garna wow'),
('122', 'gar0na wow');

DROP TABLE IF EXISTS `chat_censure_log`;
CREATE TABLE `chat_censure_log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `account` int(10) unsigned NOT NULL DEFAULT '0',
  `text` text,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `premium`;
CREATE TABLE `premium` (
  `AccountId` int(11) unsigned NOT NULL,
  `active` int(11) unsigned NOT NULL DEFAULT '1'
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='VIP';
