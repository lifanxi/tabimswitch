/*
 * Copyright (c) 2007 ftofficer <ftofficer.zhangc@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * */

var idx = 0;

var prefObserver =
{
  init: function()
  {
    this.prefs = Components.classes["@mozilla.org/preferences-service;1"]
       .getService(Components.interfaces.nsIPrefService)
       .getBranch("extensions.tabimswitch.");       
    this.prefs.QueryInterface(Components.interfaces.nsIPrefBranch2);
    this.prefs.addObserver("", prefObserver, false);

    this._loadInitPref();
  },
  
  observe: function(subject, topic, data)
  {
    if (topic != "nsPref:changed")
      return;
    
    if(    data =="debug.level" 
        || data == "debug.prefix"
        || data == "debug.log_to_file" )
    {
      this._updateDebugPref();
    }
  },
  
  _loadInitPref: function()
  {
    if ( ! this.prefs )
      return;
      
    this._updateDebugPref();
  },

  _updateDebugPref: function()
  {
    this.debugLevel = this.prefs.getIntPref("debug.level");
    this.debugPrefix = this.prefs.getCharPref("debug.prefix");
    this.logToFile = this.prefs.getBoolPref("debug.log_to_file");
        
    if ( this.logToFile )
    {
      try
      {
        var file = Components.classes["@mozilla.org/file/directory_service;1"]
                       .getService(Components.interfaces.nsIProperties)
                       .get("ProfD", Components.interfaces.nsIFile);
        file.append("tabimswitch-debuglog.txt");
        this.logFilePath = file.path;
      }
      catch (e) 
      {
        this.logToFile = false;
        this.logFilePath = null;
      }
    }
    
    tabimswitch.updateDebugSystem();
  },
  
  //
  // Members accessable from tabimswitch.
  //
  debugLevel: 2,
  debugPrefix: "TabImSwitch",

  __dummy: function(){}
};

var tabimswitch = {

  //
  // Data members
  //
  _isWndActive : false,
  _currentTab : null,

  _managedWindows : [],
  _inputMethodMap : null,

  _defaultInputMethod : null,

  _urlBarFocused : false,

  //
  // Firefox event handlers
  //
  onWindowLoad: function(e)
  {
    debugging.trace("onWindowLoad");
    this._delayedInit();
    this._removeDelayedInitEvent();
  },

  onWindowFocus: function(e)
  {
    debugging.trace("onWindowFocus");
    this.notifyTabChange(getBrowser());
    // if ( ! e.bubbles )
      this.notifyWindowChange(true);
  },

  onWindowLoseFocus: function(e)
  {
    debugging.trace("onWindowLoseFocus");
    this.notifyTabChange(getBrowser());
    // if ( ! e.bubbles )
      this.notifyWindowChange(false);
  },

  onWindowUnload: function(e)
  {
    debugging.trace("onWindowUnload");
    this.notifyTabChange(getBrowser());
  },

  onTabDocLoad: function(e)
  {
    debugging.trace("onTabDocLoad");
    this.notifyTabChange(getBrowser());
  },

  onTabAdded: function(e)
  {
    debugging.trace("onTabAdded");

    var newTab = e.target;
    var browser = newTab.linkedBrowser;

    if ( newTab )
    {
      debugging.infoLog("New Tab created: "+tabimswitch._getTabLinkedPageId(newTab));
      var im = this._manager.defaultInputMethod;
      this._saveTabInputMethod(newTab, im);
    }
    else
    {
      debugging.debugLog("No cur tab when creating new tab.");
    }

    this.notifyTabChange(browser);
  },

  onTabMoved: function(e)
  {
    debugging.trace("onTabMoved");
    var browser = e.target.linkedBrowser;
    this.notifyTabChange(browser);
  },

  onTabRemoved: function(e)
  {
    debugging.trace("onTabRemoved");
    var browser = e.target.linkedBrowser;
    this.notifyTabChange(browser);
  },

  onTabSelectChange: function(e)
  {
    debugging.trace("onWindowFocus");
    var browser = e.target.linkedBrowser;
    this.notifyTabChange(browser);
  },

  onUrlBarFocus: function(e)
  {
    debugging.trace("onUrlBarFocus");
    if ( ! this._urlBarFocused )
    {
      debugging.infoLog("Focused to URL bar");
      this._urlBarFocused = true;

      var curTab = this._currentTab;
      if ( curTab )
      {
        curIm = this._getBrowserInputMethod();
        this._saveTabInputMethod(curTab, curIm);
      }

      this._setBrowserInputMethod(this._manager.defaultInputMethod);
    }
  },

  onUrlBarLoseFocus: function(e)
  {
    debugging.trace("onUrlBarLoseFocus");

    if ( this._urlBarFocused )
    {
      debugging.infoLog("URL bar lost focus");
      this._urlBarFocused = false;
      var curTab = this._getCurrentTab(getBrowser());
      if ( curTab )
      {
        curIm = this._getTabInputMethod(curTab);
        this._setBrowserInputMethod(curIm);
        this._currentTab = curTab;
      }
      else
      {
        debugging.warning("onUrlBarLoseFocus: No current tab now");
      }
    }
  },

  onNewNavigator: function(e)
  {
    debugging.trace("onNewNavigator");
    var curTab = this._getCurrentTab(getBrowser());
    if ( curTab )
    {
      curIm = this._getBrowserInputMethod();
      this._saveTabInputMethod(curTab, curIm);
    }
    else
    {
      debugging.warning("onNewNavigator: No current tab now");
    }
  },

  notifyTabChange: function(browser)
  {
    var oldTab = this._currentTab;
    var newTab = this._getCurrentTab(browser);

    if ( newTab != null && oldTab != newTab )
    {
      debugging.infoLog("Selected Tab changed from "
           + tabimswitch._getTabLinkedPageId(oldTab) +" to "
           + tabimswitch._getTabLinkedPageId(newTab));

      //
      // If URL bar is currently focused, and the tab changed,
      // will not save the tab input method, because currently,
      // the browser input method is the default input method.
      //
      if ( oldTab != null && ! this._urlBarFocused )
      {
        oldIm = this._getBrowserInputMethod();
        this._saveTabInputMethod(oldTab, oldIm);
      }

      newIm = this._getTabInputMethod(newTab);
      this._setBrowserInputMethod(newIm);
      this._currentTab = newTab;
    }
  },

  notifyWindowChange: function(active)
  {
    if ( this._urlBarFocused )
      return;

    if ( ! this._isWndActive && active )
    {
      debugging.debugLog("Window changed from inactive to active.");

      var tab = tabimswitch._getCurrentTab(getBrowser());
      if ( tab )
      {
        debugging.debugLog("Window changed notification, active tab: "
             + tabimswitch._getTabLinkedPageId(tab));

        var im = tabimswitch._getTabInputMethod(tab);
        this._setBrowserInputMethod(im);
        this._currentTab = tab;
      }
    }
    else if ( this._isWndActive && !active )
    {
      debugging.debugLog("Window changed from active to inactive.");

      var tab = tabimswitch._getCurrentTab(getBrowser());
      if ( tab )
      {
        var tabIm = this._getBrowserInputMethod();
        this._saveTabInputMethod(tab, tabIm);
        this._currentTab = tab;

        debugging.debugLog("Window lost focus, save IM for last active tab: <"
             + tabimswitch._getTabLinkedPageId(tab) + "," + tabIm +">");
      }
     }

    this._isWndActive = active;
  },

  //
  // TabImSwitch functions
  //
  init: function()
  {
    if ( this.initialized )
      return;

    this.strings = document.getElementById("tabimswitch-strings");

    this._app = null;
    this._inputMethodMap = new Array();

    this._registerDelayedInitEvent();

    this.initialized = true;
  },

  //
  // Delayed Initialize operations after window object is created.
  // Because some operations such as XPConnect Component object is only
  // avialble after window load finished, so this function will be called
  // in the window load event handler.
  //
  _delayedInit: function()
  {
    if ( this._delayedInitSucceeded )
      return;

    prefObserver.init();

    this._app = this._getTabImSwitchApp();
    if ( this._app == null )
    {
      debugging.fatalError("init failed: cannot get TabImSwitch Application object.");
      this.initialized = false;
      return;
    }

    this._manager = this._createInputMethodManager();
    if ( this._manager == null )
    {
      debugging.fatalError("init failed: cannot get TabImSwitch Application object.");
      this.initialized = false;
      return;
    }

    if ( this._manager.defaultInputMethod == null )
    {
      this._manager.defaultInputMethod = this._getBrowserInputMethod();
    }
    else
    {
      this._setBrowserInputMethod(this._manager.defaultInputMethod);
    }

    this._registerWindowEventListeners();
    this._registerURLBarEventListeners();
    this._registerTabEventListeners();
    this._registerCommandEventListeners();

    this._delayedInitSucceeded = true;

    return;
  },
  
  updateDebugSystem: function()
  {
    if ( prefObserver.logToFile )
    {
      debugFileWriter.init(prefObserver.debugLevel, prefObserver.logFilePath);
      debugging.init(prefObserver.debugPrefix, prefObserver.debugLevel, debugFileWriter);
    }
    else
    {
      consoleWriter.init();
      debugging.init(prefObserver.debugPrefix, prefObserver.debugLevel, consoleWriter);
    }
  },

  //
  // Browser Input Method operations
  //
  _setBrowserInputMethod: function(imName)
  {
    if ( this._app )
    {
      try
      {
        if ( imName != null )
        {
          this._app.currentKeyboardLayoutName = imName;
          debugging.infoLog("Browser Input Method set to " + imName);
        }
        return true;
      }
      catch(ex)
      {
        debugging.error("_setBrowserInputMethod failed.");
      }
    }
    else
    {
      debugging.fatalError("_setBrowserInputMethod failed:: this._app==null. The TabImSwitchApp is not created.");
    }

    return false;
  },

  _getBrowserInputMethod: function()
  {
    if ( this._app )
    {
      curImName = this._app.currentKeyboardLayoutName;
      if ( curImName )
      {
        debugging.trace("Browser Input Method is " + curImName);
        return curImName;
      }
      else
      {
        debugging.fatalError("_getBrowserInputMethod failed: Can't create a new Input Method Context.");
      }
    }
    else
    {
      debugging.fatalError("_getBrowserInputMethod failed: this._app==null. The Input Method Engine is not created.");
    }

    return null;
  },

  //
  // Tab-based Input method getter and setter
  //
  _getTabInputMethod: function(tab)
  {
    if ( ! tab ) return null;
    if ( ! this._manager ) return null;

    if ( this._inputMethodMap )
    {
      tabName = this._getTabLinkedPageId(tab);
      if ( tabName )
      {
        try
        {
          var im = this._manager.getTabInputMethod(tabName);
          debugging.infoLog("Get tab input method: <"+tabName+","+im+">");
          return im;
        }
        catch (ex)
        {
          debugging.warning("Not able to get input method for <"+tabName+">");
          return null;
        }
      }
      else
      {
        debugging.error("_getTabInputMethod failed: tab does not have a linked page.")
      }
    }
    else
    {
      debugging.error("_getTabInputMethod failed: _inputMethodMap not defined.");
    }

    return null;
  },

  _saveTabInputMethod: function(tab, im)
  {
    if ( ! this._manager ) return;

    if ( tab )
    {
      if ( im != null )
      {
        if ( this._inputMethodMap )
        {
          tabName = this._getTabLinkedPageId(tab);
          if ( tabName )
          {
            try
            {
              this._manager.setTabInputMethod(tabName, im);
              debugging.infoLog("Save tab Input Method: <"+tabName+","+im+">");
            }
            catch (ex )
            {
              debugging.warning("Not able to save tab input method <"+tabName+","+im+">");
            }
          }
          else
          {
            debugging.fatalError("_saveTabInputMethod failed: tab does not have a linked page.");
          }
        }
        else
        {
          debugging.fatalError("_saveTabInputMethod failed: _inputMethodMap is not created.");
        }
      }
      else
      {
        debugging.warning("_saveTabInputMethodEx failed: will set input method to null. The operation is ignored.");
      }
    }
    else
    {
      debugging.warning("_saveTabInputMethod: not tab to save.");
    }
  },

  //
  // Helper functions
  //
  _getTabImSwitchApp: function()
  {
    try
    {
      var cid = "@tabimswitch.googlecode.com/application;1";
      var obj = Components.classes[cid].createInstance();
      obj = obj.QueryInterface(Components.interfaces.ITabImSwitchApp);
      return obj;
    }
    catch(ex)
    {
      debugging.fatalError("TabImSwitch: cannot create TabImSwitchApp: "+ex.message);
      return null;
    }

    return null;
  },

  _createInputMethodManager: function()
  {
    try
    {
      var cid = "@tabimswitch.googlecode.com/manager;1";
      var obj = Components.classes[cid].createInstance();
      obj = obj.QueryInterface(Components.interfaces.ITabInputMethodManager);
      return obj;
    }
    catch (ex)
    {
      debugging.fatalError("Cannot create TabInputMethodManager: "+ex.message);
      return null;
    }
  },

  _registerDelayedInitEvent: function()
  {
    window.addEventListener("load", tabimswitch._delayInitListener, false);
  },

  _delayInitListener: function(e) {tabimswitch.onWindowLoad(e);},
  
  _removeDelayedInitEvent: function()
  {
    window.removeEventListener("load", tabimswitch._delayInitListener, false);
  },

  _registerWindowEventListeners: function()
  {
    window.addEventListener("unload", function(e){tabimswitch.onWindowUnload(e);}, false);
    window.addEventListener("focus", function(e){tabimswitch.onWindowFocus(e);}, false);
    window.addEventListener("blur", function(e){tabimswitch.onWindowLoseFocus(e);}, true);
  },

  _registerTabEventListeners: function()
  {
    var b = getBrowser();
    b.addEventListener("load", function(e){tabimswitch.onTabDocLoad(e);}, false);

    var container = b.tabContainer;

    container.addEventListener("TabOpen", function(e){tabimswitch.onTabAdded(e);}, false);
    container.addEventListener("TabMove", function(e){tabimswitch.onTabMoved(e);}, false);
    container.addEventListener("TabClose", function(e){tabimswitch.onTabRemoved(e);}, false);
    container.addEventListener("TabSelect", function(e){tabimswitch.onTabSelectChange(e);}, false);
  },

  _registerURLBarEventListeners: function()
  {
    var url = document.getElementById("urlbar");
    url.addEventListener("focus", function(e){tabimswitch.onUrlBarFocus(e);}, true);
    url.addEventListener("blur", function(e){tabimswitch.onUrlBarLoseFocus(e);}, true);
  },

  _registerCommandEventListeners: function()
  {
    var cmdNewWin = document.getElementById("cmd_newNavigator");
    cmdNewWin.addEventListener("command", function(e){tabimswitch.onNewNavigator(e);},false);
  },

  _getWindowFromObj: function(obj)
  {
    if( !obj ) return null;

    const ci = Components.interfaces;
    var wnd = null;

    if( obj instanceof ci.nsIDOMWindow )
    {
      wnd = obj;
    }
    else if ( obj instanceof ci.nsIDOMDocument )
    {
      wnd = obj.defaultView;
    }
    else if ( obj instanceof ci.nsIDOMNode )
    {
      wnd = obj.ownerDocument.defaultView;
    }
    else
    {
      wnd = null;
    }

    if(!wnd) return null;

    return wnd.top;
  },

  _getCurrentTab: function(browser)
  {
    if ( ! browser )
      browser = getBrowser();

    var tabs = browser.tabContainer;
    if ( ! tabs )
      return null;

    return tabs.selectedItem;
  },

  _getTabLinkedPageId: function(tab)
  {
    if ( !tab )
      return null;
    docName = tab.getAttribute("linkedpanel");
    return docName;
  },

  __dummy: function() {}
};

tabimswitch.init();
