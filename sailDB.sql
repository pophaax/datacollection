/* ÅSR DB */

-- DROP DATABASE IF EXISTS asrDB;
-- CREATE DATABASE asrDB;
-- USE asrDB;

DROP DATABASE IF EXISTS jpaez;
CREATE DATABASE jpaez;
USE jpaez;

DROP TABLE IF EXISTS waypoints;
DROP TABLE IF EXISTS logs;
DROP TABLE IF EXISTS configs;
 
CREATE TABLE configs (
	# PK
	id BIGINT PRIMARY KEY AUTO_INCREMENT,

	# Sail command
	sc_commandclosereach INTEGER,
	sc_commandbeamreach INTEGER,
    sc_commandbroadreach INTEGER,
    sc_commandrunning INTEGER,
    sc_anglebeamreach INTEGER,
    sc_anglebroadreach INTEGER,
    sc_anglerunnning INTEGER,

    # Rudder command
    rc_commandextreme INTEGER,
    rc_commandmedium INTEGER,
    rc_commandsmall INTEGER,
    rc_commandmidships INTEGER,
    rc_anglemedium INTEGER,
    rc_anglesmall INTEGER,
    rc_anglemidships INTEGER,

    # Course calculator
    cc_tackangle INTEGER,
    cc_sectorangle INTEGER,

    # Wind sensor
    ws_sensormodel VARCHAR(20),
    ws_portname VARCHAR(20),
    ws_baudrate INTEGER,

    # Maestro controller
    mc_portname VARCHAR(20),

    # Rudder servo
    rs_channel INTEGER,
    rs_speed INTEGER,
    rs_acceleration INTEGER,
    rs_limitmin INTEGER,
    rs_limitmax INTEGER,

    # Sail servo
    ss_channel INTEGER,
    ss_speed INTEGER,
    ss_acceleration INTEGER,
    ss_limitmin INTEGER,
    ss_limitmax INTEGER,

    # GPS
    gps_portname VARCHAR(20),
    gps_connectionname VARCHAR(20)

) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE logs (
	# PK
	id INTEGER PRIMARY KEY AUTO_INCREMENT,

	# Time
	logtime TIMESTAMP,

	# Sail command
	sc_command INTEGER,

	# Rudder command
	rc_command INTEGER,

	# Course calculator
	cc_dtw INTEGER,
	cc_btw INTEGER,
	cc_cts INTEGER,
	cc_twd INTEGER,
	cc_tack INTEGER,

	# Wind sensor
	ws_buffersize INTEGER,
	ws_sensormodel VARCHAR(20),
	ws_direction INTEGER,
	ws_speed INTEGER,
	ws_temperature INTEGER,

	# Rudder servo
	rs_position INTEGER,

	# Sail servo
	ss_position INTEGER,

	# GPS
	gps_timestamp VARCHAR(20),
	gps_latitude DOUBLE,
	gps_longitude DOUBLE,
	gps_altitude DOUBLE,
	gps_speed DOUBLE,
	gps_heading DOUBLE,
	gps_mode INTEGER,
	gps_satellites INTEGER

) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE waypoints (
	# PK
	id INTEGER PRIMARY KEY AUTO_INCREMENT,

	# Route identifier
	routeid INTEGER,

	# waypoint config
	latitude DOUBLE,
	longitude DOUBLE

) ENGINE=MyISAM DEFAULT CHARSET=latin1;
