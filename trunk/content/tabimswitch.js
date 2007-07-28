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

var tabimswitch = {

  //
  // Data members
  //
  _isWndActive : true,
  _currentTab : null,

  _managedWindows : [],
  _inputMethodMap : null,

  //
  // Firefox event handlers
  //
  onWindowLoad: function(e)
  {
    debugging.trace("onWindowLoad");

    this._delayedInit();

    var firstTab = this._getCurrentTab(getBrowser());
    var im = this._getBrowserInputMethod();
    this._saveTabInputMethod(firstTab, im);
  },

  onWindowFocus: function(e)
  {
    debugging.trace("onWindowFocus");
    this.notifyTabChange(getBrowser());
    if ( ! e.isBubble )
      this.notifyWindowChange(true);
  },

  onWindowLoseFocus: function(e)
  {
    debugging.trace("onWindowLoseFocus");
    this.notifyTabChange(getBrowser());
    if ( ! e.isBubble )
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
      var im = this._getBrowserInputMethod();
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

  notifyTabChange: function(browser)
  {
    var oldTab = this._currentTab;
    var newTab = tabimswitch._getCurrentTab(browser);

    if ( newTab != null && oldTab != newTab )
    {
      debugging.infoLog("Selected Tab changed from "
           + tabimswitch._getTabLinkedPageId(oldTab) +" to "
           + tabimswitch._getTabLinkedPageId(newTab));

      if ( oldTab != null )
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
    if ( ! this._isWndActive && active )
    {
      // debugging.debugLog("Window changed from inactive to active.");
      /* var tab = tabimswitch._getCurrentTab(getBrowser());
      if ( tab )
      {
        var imc = tabimswitch._getTabIMC(tab);
        debugging.debugLog("Window changed notification, active tab: "
             + tabimswitch._getTabLinkedPageId(tab) + "[" + (imc?imc.debugId:-1) + "]" );

        this._setCurrentIMC(tab);
      }*/
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

    this._registerWindowEventListeners();

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

    this._app = this._getTabImSwitchApp();
    if ( this._app == null )
    {
      debugging.fatalError("init failed: cannot get TabImSwitch Application object.");
      this.initialized = false;
      return;
    }

    this._registerTabEventListeners();

    this._delayedInitSucceeded = true;

    return;
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
        debugging.debugLog("Browser Input Method is " + curImName);
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

    if ( this._inputMethodMap )
    {
      tabName = this._getTabLinkedPageId(tab);
      if ( tabName )
      {
        var im = this._inputMethodMap[tabName];
        debugging.infoLog("Get tab input method: <"+tabName+","+im+">");
        return im;
      }
      else
      {
        debugging.error("_getTabIMC failed: tab does not have a linked page.")
      }
    }
    else
    {
      debugging.error("_getTabIMC failed: _inputMethodMap not defined.");
    }

    return null;
  },

  _saveTabInputMethod: function(tab, im)
  {
    if ( tab )
    {
      if ( im != null )
      {
        if ( this._inputMethodMap )
        {
          tabName = this._getTabLinkedPageId(tab);
          if ( tabName )
          {
            this._inputMethodMap[tabName] = im;
            debugging.infoLog("Save tab Input Method: <"+tabName+","+im+">");
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
      var cid = "@tabimswitch.sourceforge.net/application;1";
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

  _registerWindowEventListeners: function()
  {
    window.addEventListener("load", function(e){tabimswitch.onWindowLoad(e);}, false);
    window.addEventListener("focus", function(e){tabimswitch.onWindowFocus(e);}, false);
    window.addEventListener("blur", function(e){tabimswitch.onWindowLoseFocus(e);}, true);
    window.addEventListener("unload", function(e){tabimswitch.onWindowUnload(e);}, false);
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
