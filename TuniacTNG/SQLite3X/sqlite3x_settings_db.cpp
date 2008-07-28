
#include "sqlite3x_settings_db.hpp"

namespace sqlite3x
{

	settings_db::settings_db()
		: m_db( 0 )
	{
	}

	settings_db::settings_db( std::string const & dbname )
		: m_db( 0 )
	{
		this->open( dbname );
	}

	settings_db::~settings_db()
	{
		try
		{
			this->close();
		}
		catch(...)
		{
		}
	}

	bool settings_db::is_open() const
	{
		return 0 != this->m_db;
	}

	void settings_db::open( std::string const & dbname )
	{
		this->close();
		this->m_db = new sqlite3_connection( dbname );
		this->init();
	}

	void settings_db::close()
	{
		if( this->m_db )
		{
			this->m_db->close();
			delete this->m_db;
		}
		this->m_db = 0;
	}

	void settings_db::clear()
	{
		this->m_db->executenonquery( "delete from settings" );
	}

	void settings_db::clear( std::string const & where )
	{
		this->m_db->executenonquery( "delete from settings " + where );
	}


	sqlite3_connection * settings_db::db()
	{
		return this->m_db;
	}

	static std::string SettingsDb_Set_SQL = "insert into settings values(?,?)";

	void settings_db::set( std::string const & key, int val )
	{
		sqlite3_command st( *this->m_db, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val );
		st.executenonquery();
	}

	void settings_db::set( std::string const & key, sqlite_int64 val )
	{
		sqlite3_command st( *this->m_db, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val );
		st.executenonquery();
	}

	void settings_db::set( std::string const & key, bool val )
	{
		sqlite3_command st( *this->m_db, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val ? 1 : 0 );
		st.executenonquery();
	}

	void settings_db::set( std::string const & key, double val )
	{
		sqlite3_command st( *this->m_db, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val );
		st.executenonquery();
	}

	void settings_db::set( std::string const & key, std::string const & val )
	{
		sqlite3_command st( *this->m_db, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val );
		st.executenonquery();
	}

	void settings_db::set( std::string const & key, char const * val )
	{
		sqlite3_command st( *this->m_db, SettingsDb_Set_SQL );
		st.bind( 1, key );
		st.bind( 2, val ? val : "" );
		st.executenonquery();
	}

	void settings_db::init()
	{
		this->m_db->executenonquery( "create table if not exists settings(key PRIMARY KEY ON CONFLICT REPLACE,value)" );
		//this->m_db->executenonquery( "PRAGMA temp_store = MEMORY" ); // i don't like this, but want to speed up access
		//this->m_db->executenonquery( "PRAGMA synchronous = OFF" ); // again: i don't like this but want more speed
	}

	static std::string SettingsDb_Get_SQL = "select value from settings where key = ?";

	bool settings_db::get( std::string const & key, int & val )
	{
		try
		{
			sqlite3_command st( *this->m_db, SettingsDb_Get_SQL );
			st.bind( 1, key );
			val = st.executeint();
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}

	bool settings_db::get( std::string const & key, sqlite_int64 & val )
	{
		try
		{
			sqlite3_command st( *this->m_db, SettingsDb_Get_SQL );
			st.bind( 1, key );
			val = st.executeint64();
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}
	bool settings_db::get( std::string const & key, bool & val )
	{
		try
		{
			sqlite3_command st( *this->m_db, SettingsDb_Get_SQL );
			st.bind( 1, key );
			val = (st.executeint() ? true : false);
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}
	bool settings_db::get( std::string const & key, double & val )
	{
		try
		{
			sqlite3_command st( *this->m_db, SettingsDb_Get_SQL );
			st.bind( 1, key );
			val = st.executedouble();
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}
	bool settings_db::get( std::string const & key, std::string & val )
	{
		try
		{
			sqlite3_command st( *this->m_db, SettingsDb_Get_SQL );
			st.bind( 1, key );
			val = st.executestring();
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}



} // namespace
