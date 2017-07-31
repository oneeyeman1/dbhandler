## Process this file with automake to produce Makefile.in


lib_LTLIBRARIES =  \
	libdbloader.la

libdbloader_la_SOURCES =  \
	dbloader.cpp

libdbloader_la_CXXFLAGS = -I/usr/include \
	-I../../dbinterface \
	-I../../libsqlite \
	-I../../libodbc \
	-I../../libpostgres \
	-I../../libmysql \
	-I../../mysql-connector/include \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags` 

libdbloader_la_LDFLAGS = -L../libsqlite \
	-L../libodbc \
	-L../libpostgres \
	-L../libmysql \
	-L../../mysql-connector/lib \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs` \
	-lsqlite_lib \
	-lodbc_lib \
	-lpostgres \
	-lmysqlclient \
	-lmysql_lib
	 
	 
