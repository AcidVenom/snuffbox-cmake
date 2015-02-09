#pragma once

#include <map>
#include <string>

namespace snuffbox
{
	/**
	* @class snuffbox::CVar
	* @brief A class to register CVars with and to parse the command line for CVars
	* @author Daniël Konings
	*/
	class CVar
	{
	public:
		/**
		* @enum snuffbox::CVar::ValueTypes
		* @brief An enumeration that contains the different CVar types
		* @author Daniël Konings
		*/
		enum ValueTypes
		{
			kBoolean,
			kNumber,
			kString
		};

		/**
		* @class snuffbox::CVar::Value
		* @brief The base class of every CVar value
		* @author Daniël Konings
		*/
		class Value
		{
		public:
			/// Default constructor
			Value(ValueTypes type);

			/// Default destructor
			virtual ~Value();

			/**
			* @return bool Is this value a boolean?
			*/
			bool IsBool();

			/**
			* @return bool Is this value a number?
			*/
			bool IsNumber();

			/**
			* @return bool Is this value a string?
			*/
			bool IsString();

			/**
			* @brief Retrieves the given type of this CVar value
			* @return snuffbox::CVar::ValueTypes The type of this value
			*/
			ValueTypes type();

			/**
			* @brief Dynamically cast this value to a downcasted value
			* @return T* The downcasted value (nullptr if conversion was invalid)
			*/
			template<typename T>
			T* As();

		private:
			ValueTypes type_; //!< The type of this value
		};

		class Boolean : public Value
		{
		public:
			/**
			* @brief Construct through value
			* @param[in] value (bool) The boolean value to be associated with this CVar
			*/
			Boolean(bool value);

			/// Default destructor
			virtual ~Boolean();

			/**
			* @return const bool& The value associated with this CVar
			*/
			const bool& value() const;

			/**
			* @brief Sets the value
			* @param[in] value (bool) The value to set
			*/
			void set_value(bool value);

		private:
			bool value_; //!< The boolean value
		}; 

		class Number : public Value
		{
		public:
			/**
			* @brief Construct through value
			* @param[in] value (double) The double value to be associated with this CVar
			*/
			Number(double value);

			/// Default destructor
			virtual ~Number();

			/**
			* @return const double& The value associated with this CVar
			*/
			const double& value() const;

			/**
			* @brief Sets the value
			* @param[in] value (double) The value to set
			*/
			void set_value(double value);

		private:
			double value_; //!< The double value
		};

		class String : public Value
		{
		public:
			/**
			* @brief Construct through value
			* @param[in] value (std::string) The string value to be associated with this CVar
			*/
			String(std::string value);

			/// Default constructor
			virtual ~String();

			/** 
			* @return const std::string& The value associated with this CVar
			*/
			const std::string& value() const;

			/**
			* @brief Sets the value
			* @param[in] value (std::string) The value to set
			*/
			void set_value(std::string value);

		private:
			std::string value_; //!< The string value
		};

		/// Default constructor
		CVar();

		/// Default destructor
		~CVar();

		/**
		* @brief Retrieves the singleton instance of this class
		* @return snuffbox::CVar* The pointer to the singleton
		*/
		static CVar* Instance();

		/**
		* @brief Registers a boolean CVar value for a given name
		* @param[in] name (std::string) The name of the CVar
		* @param[in] value (bool) The boolean value
		*/
		void Register(std::string name, bool value);

		/**
		* @brief Registers a number CVar value for a given name
		* @param[in] name (std::string) The name of the CVar
		* @param[in] value (double) The boolean value
		*/
		void Register(std::string name, double value);

		/**
		* @brief Registers a string CVar value for a given name
		* @param[in] name (std::string) The name of the CVar
		* @param[in] value (std::string) The string value
		*/
		void Register(std::string name, std::string value);

		/**
		* @brief Maps a CVar to its corresponding map index
		* @param[in] name (const std::string&) The name of the CVar
		* @param[in] val (snuffbox::CVar::Value*) The CVar value to map
		*/
		void Map(const std::string& name, Value* val);

		/**
		* @brief Retrieves a CVar from the registered CVars
		* @param[in] name (std::string) The name of the CVar to retrieve
		* @param[in] found (bool*) Will be false if not found
		* @return (snuffbox::CVar::Value*) The corresponding value
		*/
		CVar::Value* Get(std::string name, bool* found);

		/**
		* @brief Checks if a CVar value was registered
		* @param[in] name (std::string) The CVar name to search for
		* @return bool Does it exist?
		*/
		bool Exists(std::string name);

		/**
		* @brief Parses the command line passed by the executable
		* @param[in] argc (int) The argument count
		* @param[in] argv (char**) The actual arguments
		*/
		void ParseCommandLine(int argc, char** argv);

		/**
		* @brief Registers command line arguments as CVars
		* @param[in] argc (int) The argument count
		* @param[in] argv (char**) The actual arguments
		*/
		void RegisterCommandLine(int argc, char** argv);

	private:
		typedef std::map<std::string, CVar::Value*> CVarMap;
		CVarMap vars_; //!< The variables 
	};

	template<typename T>
	T* CVar::Value::As()
	{
		return dynamic_cast<T*>(this);
	}
}