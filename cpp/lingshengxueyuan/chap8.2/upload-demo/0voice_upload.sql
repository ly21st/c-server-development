#数据库表
#创建数据库
DROP database IF EXISTS `0voice_upload`;
CREATE DATABASE `0voice_upload`;

#使用数据库
use `0voice_upload`;


DROP TABLE IF EXISTS `file_test`;
CREATE TABLE `file_test` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '',
  `file_name` varchar(256) NOT NULL COMMENT '',
  `fileid` varchar(256) NOT NULL COMMENT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=70 DEFAULT CHARSET=utf8 COMMENT='文件信息表';