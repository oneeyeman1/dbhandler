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
	`odbc_config \
	--cflags` \
	-I`pg_config \
	--includedir` \
	`mysql_config \
	--cflags` \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--cxxflags`

libdbloader_la_LDFLAGS = -L../dbinterface \
	-ldbinterface \
	-L../libsqlite \
	-L../libodbc \
	-L../libpostgres \
	-L../libmysql \
	-lsqlite_lib \
	-lodbc_lib \
	-lpostgres \
	-lmysql_lib \
	`odbc_config \
	--libs` \
	-lodbcinst \
	-L`pg_config \
	--libdir` \
	-lpq \
	`mysql_config \
	--libs` \
	`/home/igor/wxWidgets/buildGTK/wx-config \
	--libs`
