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
	-I../../libsqlany \
	@ODBC_CFLAGS@ \
	@POSTGRES_CXXFLAGS@ \
	@MYSQL_CXXFLAGS@ \
	-I@SQLANY@/sdk/include \
	`wx-config \
	--cxxflags`

libdbloader_la_LDFLAGS = -L../dbinterface \
	-ldbinterface \
	-L../libsqlite \
	-L../libodbc \
	-L../libpostgres \
	-L../libmysql \
	-L../libsqlany \
	-L@SQLANY@/lib64 \
	-lsqlite_lib \
	-lodbc_lib \
	-lpostgres \
	-lmysql_lib \
	-lsqlany \
	@ODBC_LIBS@ \
	@POSTGRES_LIBS@ \
	@MYSQL_LIBS@ \
	-ldbcapi \
	`wx-config \
	--libs`
