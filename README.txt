OVERVIEW
-------------------------------------------------
The microsite_entity module allows site administrators to set up new entities (i.e. pseudo-content-types) called Microsites.  Microsites are, for all intents, static websites which can be created and managed from within the Drupal editorial/adminisrative environment. At the present time there is only one defined type of Microsite - the Hosted Microsite. 

Hosted Microsites consits of a name, description, tags, and a directory (or directory tree) containing the files needed for the site - i.e. tml, css, javascript, images, pdfs, etc that implement the site. Other than the site file tree, the fields for a microsite are purely for adminsirative viewing - they have no bearing on the microsite itself. The Microsite entity type exists primarily as a mechanism to bulk-upload the microsite's assets.


INSTALLATION
-------------------------------------------------
Install this module as you would any other. Enbling the module will prompt you to allow the module's prerequisites also to be enabled. Thee include the imce_plup_field module, as well as several contrib modules.

The url directory for all of the microsites (www.example.com/microsites/) is configurable by changing the value for hosted_url_dir variable and changing an alias in all of the apache configuration files.  This is handled by a server administrator.  The value of the hosted_url_dir  and the microsite_hosted_path variables can be changed by editing the settings.php file and adding/changing the lines: 

$conf['hosted_url_dir'] = 'microsites';
$conf['microsite_hosted_path'] = 'home/devadmin/dcms/examplesite/microsites/files';

A file not found error for the new directory/new microsite will occur until the Alias has been set up properly.  Contact a server admin to set change this value. 

DEPENDENCIES
-------------------------------------------------
This module requires the imce_plup_field module in order to provide the ability for admins to upload microsite assets.

The imce_plup_field module itself requires contributed modules imce_lupload, plupload, imce_mkdir, imce_rename, date, date_popup, date_api, views, ctools, and entity.

CONFIGURATION
--------------------------------------------------
Before create any microsites, the base directory where microsite files will be stored must be configured. This may be done at admin/config/media/file-system, where the path (either an absolute system path beginning with a '/' (e.g. /tmp/test_hosted_files) or a relative path without a prepended '/' (e.g. 'sites/default/files/test_hosted_files'). Click the "Save Configuration' button.

CREATING/EDITING A MICROSITE
--------------------------------------------------
Go to url microsite/add. Click "Hosted microsite." Provide, at minimum, a name for the new microsite, then click 'Save microsite'. After saving, click the "Edit' tab to go back in and eit the microsite. You'll now see an IMCE file list widget with no files in it.

To add files to the microsite, click the "Upload" button at the top of the IMCE window. Another window will popup - in that window either click "Add files" and then select files to be uploaded, or drag and drop files into the "Drag files here" area of the popup window. After all files have been selected, click the "Upload" button at the bottom-left of the popup window. Once all the follows have been uploaded, you may close the popup "Add Files" window by clicking the 'X' in its upper-right corner.

Finally, click the "Save microsite" button at the bottom of the page. Once the microsite has been saved, you'll be pt on the microsite's "View" page, which will show its name, description, tags, date(s), and a table of all uploaded files you may click on any of the files to view them in the browser. You may also click on any of the tgs, which will show a table listing all curren microsites tagged with that tag. You ma further edit the microsite by clicking the "Edit" tab.
