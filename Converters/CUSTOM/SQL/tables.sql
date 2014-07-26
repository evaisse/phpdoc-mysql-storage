-- phpMyAdmin SQL Dump
-- version 3.2.5
-- http://www.phpmyadmin.net
--
-- Serveur: localhost
-- Généré le : Mar 30 Novembre 2010 à 19:36
-- Version du serveur: 5.1.44
-- Version de PHP: 5.2.13

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

--
-- Base de données: `documentation_intra`
--

-- --------------------------------------------------------

--
-- Structure de la table `doc_class`
--

CREATE TABLE `doc_class` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `package` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `extends` varchar(255) DEFAULT NULL,
  `file` varchar(1000) NOT NULL,
  `modifiers` text NOT NULL,
  `doc` text,
  PRIMARY KEY (`id`),
  UNIQUE KEY `selector` (`name`,`package`),
  KEY `name` (`name`),
  KEY `file` (`file`),
  KEY `extends` (`extends`),
  KEY `id` (`id`),
  KEY `package` (`package`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `doc_constant`
--

CREATE TABLE `doc_constant` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `package` varchar(255) NOT NULL,
  `class` varchar(255) DEFAULT NULL,
  `name` varchar(255) NOT NULL,
  `doc` text,
  `value` text,
  `line` int(255) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `selector` (`name`,`class`,`package`),
  KEY `package` (`package`),
  KEY `class` (`class`),
  KEY `name` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `doc_function`
--

CREATE TABLE `doc_function` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `package` varchar(255) NOT NULL,
  `class` varchar(255) DEFAULT NULL,
  `name` varchar(255) NOT NULL,
  `doc` text NOT NULL,
  `signature` text NOT NULL,
  `modifiers` text NOT NULL,
  `line` int(10) NOT NULL,
  `return` text NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `selector` (`name`,`class`,`package`),
  KEY `package` (`package`),
  KEY `class` (`class`),
  KEY `name` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `doc_parameter`
--

CREATE TABLE `doc_parameter` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `package` varchar(255) NOT NULL,
  `class` varchar(255) NOT NULL,
  `function` varchar(255) NOT NULL,
  `name` varchar(100) NOT NULL,
  `type` varchar(255) DEFAULT NULL,
  `default` text,
  `pos` tinyint(2) NOT NULL,
  `doc` text,
  PRIMARY KEY (`id`),
  UNIQUE KEY `selector` (`name`,`class`,`function`, `package`),
  KEY `name` (`name`),
  KEY `class` (`class`),
  KEY `function` (`function`),
  KEY `package` (`package`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `doc_property`
--

CREATE TABLE `doc_property` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `package` varchar(255) NOT NULL,
  `class` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `doc` text,
  `modifiers` varchar(255) NOT NULL,
  `value` text,
  `line` int(10) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `selector` (`name`,`class`,`package`),
  KEY `package` (`package`),
  KEY `class` (`class`),
  KEY `name` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `doc_source`
--

CREATE TABLE `doc_source` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `package` varchar(255) CHARACTER SET latin1 NOT NULL,
  `class` varchar(255) CHARACTER SET latin1 NOT NULL,
  `method` varchar(255) CHARACTER SET latin1 NOT NULL,
  `source` text CHARACTER SET latin1 NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `selector` (`method`,`class`,`package`),
  KEY `package` (`package`),
  KEY `class` (`class`),
  KEY `method` (`method`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `doc_tag`
--

CREATE TABLE `doc_tag` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `tag` varchar(150) NOT NULL,
  `value` text NOT NULL,
  `table` varchar(255) NOT NULL,
  `package` varchar(255) NOT NULL,
  `class` varchar(255) NOT NULL DEFAULT '',
  `name` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `selector` (`name`,`class`,`package`, `table`),
  KEY `name` (`name`),
  KEY `class` (`class`),
  KEY `table` (`table`),
  KEY `package` (`package`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;
