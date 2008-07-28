#ifndef s11n_net_SQLITE3X_SETTINGS_DB_INCLUDED
#define s11n_net_SQLITE3X_SETTINGS_DB_INCLUDED 1

#include "sqlite3x.hpp"

namespace sqlite3x {

	/**
	   settings_db ia a very simplistic settings-data database for
	   use with the sqlite3x database layer.

	   Usage:
<pre>
	   settings_db db("my.db");
	   db.set("one", 1 );
	   db.set("two", 2.0 );
	   db.set("a_string", "a string" );

	   std::string sval;
	   assert( db.get( "a_string", sval ) );
</pre>

	Obviously, an assert may be too harsh for what you're doing.

	*/
	class settings_db
	{
	public:
		/**
		   Calls open(dbname). This ctor will throw if dbname
		   cannot be opened or if it is not a database.
		*/
		explicit settings_db( std::string const & dbname );
		/**
		   Creates an unopened database. You must call open()
		   before you can use this object.
		 */
		settings_db();
		/**
		   Closes this database.
		*/
		~settings_db();
		/**
		   Returns true if open() has succeeded.
		*/
		bool is_open() const;
		/**
		   Empties the database. Does not remove the db file.
		*/
		void clear();
		/**
		   Empties the database items matching the given WHERE
		   clause. Does not remove the db file.

		   'where' should be a full SQL where statement, e.g.:

		   "WHERE KEY LIKE 'x%'"

		   The field names in this db are KEY and VALUE.
		*/
		void clear( std::string const & where );

		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, int val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, sqlite_int64 val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, bool val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, double val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, std::string const & val );
		/**
		   Sets the given key/value pair.
		 */
		void set( std::string const & key, char const * val );

		/**
		   Fetches the given key from the db. If it is found,
		   it is converted to the data type of val, val is
		   assigned that value, and true is returned. If false
		   is returned then val is unchanged.
		*/
		bool get( std::string const & key, int & val );
		/** See get(string,int). */
		bool get( std::string const & key, sqlite_int64 & val );
		/** See get(string,int). */
		bool get( std::string const & key, bool & val );
		/** See get(string,int). */
		bool get( std::string const & key, double & val );
		/** See get(string,int). */
		bool get( std::string const & key, std::string & val );

		/**
		   Opens the database dbname or throws on error.
		*/
		void open( std::string const & dbname );
		/**
		   Closes this database. Not normally necessary, as
		   this happens during the destruction of this object.
		*/
		void close();

		/**
		   If you want low-level info about the db, here's the
		   handle to it. This will be null before open() has
		   succeeded.
		 */
		sqlite3_connection * db();
	private:
		void init();
		sqlite3_connection * m_db;
	};

} // namespace whnet


#endif // s11n_net_SQLITE3X_SETTINGS_DB_INCLUDED
