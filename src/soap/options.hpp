#ifndef _SOAP_OPTIONS_HPP
#define _SOAP_OPTIONS_HPP

#include <map>
#include <boost/format.hpp>

#include "soap/types.hpp"

namespace soap {

class Options
{
public:
	typedef std::map<std::string, std::string> map_options_t;
	typedef std::map<std::string, bool> map_exclude_t;
	typedef std::map<int, bool> map_exclude_id_t;

	// CONSTRUCT & DESTRUCT
	Options();
    ~Options() {}

    // GET & SET
    template<typename return_t>
    return_t get(std::string key) {
        return soap::lexical_cast<return_t, std::string>(_key_value_map[key], "wrong or missing type in " + key);
    }
    void set(std::string key, std::string value) { _key_value_map[key] = value; }
    void set(std::string key, int value) { this->set(key, boost::lexical_cast<std::string>(value)); }
    void set(std::string key, double value) { this->set(key, boost::lexical_cast<std::string>(value)); }
    bool hasKey(std::string key) { return (_key_value_map.find(key) == _key_value_map.end()) ? false : true; }
    //void set(std::string key, bool value) { this->set(key, boost::lexical_cast<std::string>(value)); }
	//void configureCenters(boost::python::list center_excludes) { _center_excludes = center_excludes; }
	std::string summarizeOptions();

	// EXCLUSIONS TODO Move this to a distinct class
	void excludeCenters(boost::python::list &types);
	void excludeTargets(boost::python::list &types);
	bool doExcludeCenter(std::string &type);
	bool doExcludeTarget(std::string &type);
	boost::python::list getExcludeCenterList() { return _exclude_center_list; }
	boost::python::list getExcludeTargetList() { return _exclude_target_list; }

	void excludeCenterIds(boost::python::list &pids);
	void excludeTargetIds(boost::python::list &pids);
	bool doExcludeCenterId(int pid);
	bool doExcludeTargetId(int pid);
	boost::python::list getExcludeCenterIdList() { return _exclude_center_id_list; }
	boost::python::list getExcludeTargetIdList() { return _exclude_target_id_list; }

	// PYTHON
	static void registerPython();

	// SERIALIZATION
	template<class Archive>
	void serialize(Archive &arch, const unsigned int version) {
		arch & _key_value_map;

		arch & _exclude_center;
		arch & _exclude_target;
		arch & _exclude_center_list;
		arch & _exclude_target_list;

		arch & _exclude_center_id;
		arch & _exclude_target_id;
		arch & _exclude_center_id_list;
		arch & _exclude_target_id_list;
		return;
	}

private:
	boost::python::list _center_excludes;
	map_options_t _key_value_map;

	map_exclude_t _exclude_center;
	map_exclude_t _exclude_target;
	boost::python::list _exclude_center_list;
    boost::python::list _exclude_target_list;

    map_exclude_id_t _exclude_center_id;
    map_exclude_id_t _exclude_target_id;
    boost::python::list _exclude_center_id_list;
    boost::python::list _exclude_target_id_list;
};

}

#endif
