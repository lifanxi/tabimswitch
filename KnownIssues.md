# Known Issues #

Other Language **[简体中文](KnownIssuesZhcn.md)**

## All versions ##

### The input method is not able to be saved on Windows XP and Vista. ###
**Behavior:** In some system, the input method of one tab may not be able to be saved. For example, you change the Input Method to MSPY 2003 at one tab, and change to another tab. The Input method is closed as expected. But when you change back to the first tab, input method is not changed back accordingly.

**Solution:** On Windows XP SP2, please close advanced text service for Firefox. Try the following steps:
  1. Select "Regional and Language Options" in Control Panel.
  1. Click "Details" at "Language" tab.
  1. In the "Text Service and Input Languages" window, **uncheck** the check box "Extend support of advanced text services to all programs".

![http://tabimswitch.googlecode.com/files/deselect-advanced-text-services.png](http://tabimswitch.googlecode.com/files/deselect-advanced-text-services.png)
