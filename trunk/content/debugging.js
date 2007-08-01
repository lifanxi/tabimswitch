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

//
// Debugging support object.
//
var debugging = {

  LOG_FATAL: 0,
  LOG_ERROR: 1,
  LOG_WARN:  2,
  LOG_INFO:  3,
  LOG_DEBUG: 4,
  LOG_TRACE: 5,

  _logLevelName: ['Fatal', 'Error', 'Warning', 'Information', 'Debugging', 'Tracing'],

  //
  // Send a message to the system log
  //
  _writeLog: function(level, message)
  {
    if ( level > this._debugLevel )
      return;

    try
    {
      var log = Components.classes["@mozilla.org/consoleservice;1"]
                          .getService(Components.interfaces.nsIConsoleService);
      log.logStringMessage(this._prefix+": ["+this._logLevelName[level]+"] "+message);
    }
    catch(e)
    {
      Components.reportError(e);
    }
  },

  init : function(prefix, level)
  {
    this._prefix = prefix;
    this._debugLevel = level;
  },

  trace: function(msg) { this._writeLog(this.LOG_TRACE, msg); },
  debugLog: function(msg) { this._writeLog(this.LOG_DEBUG, msg); },
  infoLog: function(msg) { this._writeLog(this.LOG_INFO, msg); },
  warning: function(msg) { this._writeLog(this.LOG_WARN, msg); },
  error: function(msg) { this._writeLog(this.LOG_ERROR, msg); },
  fatalError: function(msg) { this._writeLog(this.LOG_FATAL, msg); },

  __dummy : function() {}
};

debugging.init("TabImSwitch", debugging.LOG_TRACE);
