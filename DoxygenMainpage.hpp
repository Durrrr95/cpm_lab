//Main page design
/** \mainpage CPM Lab Software
 * 
 * \section intro Introduction
 * Welcome to the official Doxygen Documentation for the CPM Lab Software. 
 * 
 * Use the search bar to search for further information for a particular class,
 * or take a look at the categories below to find out more about the code modules.
 * 
 * Please refer to the official Confluence documentation https://cpm.embedded.rwth-aachen.de/doc/
 * for a detailed explanation of the usage and ideas behind these files
 * 
 * \section projects Software Projects
 * \subsection middleware_section Middleware
 * You can find out more about the Middleware here: \ref middleware
 * 
 * \subsection cpm_section CPM Library
 * You can find out more about the CPM Library here: \ref cpmlib
 * 
 * \subsection lcc_section LCC
 * You can find out more about the LCC here: \ref lcc
 * 
 * \subsection lcc_ui_section LCC UI
 * You can find out more about the LCC's UI here: \ref lcc_ui
 * 
 * \subsection lcc_commonroad_section LCC Commonroad
 * You can find out more about the LCC's Commonroad files here: \ref lcc_commonroad
 * 
 * \subsection lcc_labcam_section LCC Labcam
 * You can find out more about the LCC's Labcam files here: \ref lcc_labcam
 * 
 * \subsection ips_section IPS
 * You can find out more about the IPS files here: \ref ips
 * 
 * \subsection vehicle_section Vehicle
 * You can find out more about the Vehicle files here: \ref vehicle
 * 
 * \subsection autostart_section Autostart
 */

//Namespace definitions
/**
 * \namespace cpm
 * \brief A namespace that contains all cpm lib functions and classes. See \ref cpmlib.
 */

// Group definitions
/**
 * \defgroup middleware
 * These files are part of the Middleware. 
 * 
 * You can find out more about the Middleware in https://cpm.embedded.rwth-aachen.de/doc/display/CLD/Middleware+Usage
 * The Middleware relies on QOS_LOCAL_COMMUNICATION.xml - without this configuration file next to the executable, local configuration is not configured properly
 */

/**
 * \defgroup cpmlib
 * These files are all part of the CPM Library. The library was written to be used by all other programs that require DDS, timing or some other
 * common functionality. 
 * 
 * You can find out more about it here: https://cpm.embedded.rwth-aachen.de/doc/display/CLD/CPM+Library
 */

/**
 * \defgroup lcc
 * These files are part of the LCC and not UI or Commonroad Definitions. 
 * 
 * The LCC is a graphical program for simulation and control within the Lab.
 * You can find out more in https://cpm.embedded.rwth-aachen.de/doc/display/CLD/Lab+Control+Center
 */

/**
 * \defgroup lcc_ui
 * These files are part of the LCC UI files. 
 * 
 * The LCC is a graphical program for simulation and control within the Lab.
 * You can find out more in https://cpm.embedded.rwth-aachen.de/doc/display/CLD/Lab+Control+Center
 */

/**
 * \defgroup lcc_commonroad
 * These files are part of the LCC Commonroad files, for parsing and displaying commonroad data. (Except for some additional classes in LCC UI)
 * 
 * The LCC is a graphical program for simulation and control within the Lab.
 * You can find out more in https://cpm.embedded.rwth-aachen.de/doc/display/CLD/Lab+Control+Center
 */

/**
 * \defgroup lcc_labcam
 * These files are part of the LCC Labcam files, for using the cam in the lab (if it is not used in simulated mode).
 * 
 * There is a Readme for setup: \ref LabcamReadme
 * 
 * The LCC is a graphical program for simulation and control within the Lab.
 * You can find out more in https://cpm.embedded.rwth-aachen.de/doc/display/CLD/Lab+Control+Center
 */

/**
 * \defgroup ips
 * These files are part of the Indoor Positioning System Software, which is used in combination with a camera to track the vehicles in the Lab,
 * functioning as an indoor GPS alternative.
 * 
 * You can find out more in https://cpm.embedded.rwth-aachen.de/doc/display/CLD/Indoor+Positioning+System
 */

/**
 * \defgroup vehicle
 * These files are part of the Vehicle, which is used either for the real vehicles in the Lab on the Raspberry Pis, or
 * for simulated vehicles on the PC that simulates them.
 * 
 * The vehicle also uses the Broadcom library, which is described here: \ref broadcom_page
 * 
 * You can find out more in https://cpm.embedded.rwth-aachen.de/doc/display/CLD/Vehicle
 */