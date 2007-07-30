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

const ITabInputMethodManager = Components.interfaces.ITabInputMethodManager;
const nsISupports = Components.interfaces.nsISupports;

// UUID uniquely identifying our component
const CLASS_ID = Components.ID("{d047ba54-da67-402c-a067-193829ff8699}");

// description
const CLASS_NAME = "TabIMSwitch Input Method Manager";

// textual unique identifier
const CONTRACT_ID = "@tabimswitch.sourceforge.net/manager;1";

/***********************************************************
 class definition
***********************************************************/

//class constructor
function TabInputMethodManager() {
  this._imMap = new Array();
};

// class definition
TabInputMethodManager.prototype = {

  // ITabInputMethodManager interface implementation
  setTabInputMethod: function(tabKey, inputMethod)
  {
    this._imMap[tabKey] = inputMethod;
  },

  getTabInputMethod: function(tabKey)
  {
    return this._imMap[tabKey];
  },

  QueryInterface: function(aIID)
  {
    if (!aIID.equals(ITabInputMethodManager) &&
        !aIID.equals(nsISupports))
    {
      throw Components.results.NS_ERROR_NO_INTERFACE;
    }

    return this;
  }
};

/***********************************************************
  class factory

  This object is a member of the global-scope Components.classes.
  It is keyed off of the contract ID. Eg:

  myHelloWorld = Components.classes["@dietrich.ganx4.com/helloworld;1"].
                          createInstance(Components.interfaces.nsIHelloWorld);
***********************************************************/

var TabInputMethodManagerFactory = {
  createInstance: function (aOuter, aIID)
  {
    if (aOuter != null)
      throw Components.results.NS_ERROR_NO_AGGREGATION;

    return (new TabInputMethodManager()).QueryInterface(aIID);
  }
};

/***********************************************************
  module definition (xpcom registration)
***********************************************************/
var TabInputMethodManagerModule = {

  registerSelf: function(aCompMgr, aFileSpec, aLocation, aType)
  {
    aCompMgr = aCompMgr.
        QueryInterface(Components.interfaces.nsIComponentRegistrar);
    aCompMgr.registerFactoryLocation(CLASS_ID, CLASS_NAME,
        CONTRACT_ID, aFileSpec, aLocation, aType);
  },

  unregisterSelf: function(aCompMgr, aLocation, aType)
  {
    aCompMgr = aCompMgr.
        QueryInterface(Components.interfaces.nsIComponentRegistrar);
    aCompMgr.unregisterFactoryLocation(CLASS_ID, aLocation);
  },

  getClassObject: function(aCompMgr, aCID, aIID)
  {
    if (!aIID.equals(Components.interfaces.nsIFactory))
      throw Components.results.NS_ERROR_NOT_IMPLEMENTED;

    if (aCID.equals(CLASS_ID))
      return TabInputMethodManagerFactory;

    throw Components.results.NS_ERROR_NO_INTERFACE;
  },

  canUnload: function(aCompMgr) { return false; }
};

/***********************************************************
  module initialization

  When the application registers the component, this function
  is called.
***********************************************************/
function NSGetModule(aCompMgr, aFileSpec) { return TabInputMethodManagerModule; }
