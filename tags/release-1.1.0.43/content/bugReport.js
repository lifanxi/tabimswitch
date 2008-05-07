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
 
var bugReporter = {

  checkContactInfo: function()
  {
    var submitter = document.getElementById("submitter").value;
    var email = document.getElementById("submitter-email").value;
    
    var inputAccepted = (bugReporterApp.setSubmitter(submitter, email));
    document.getElementById('wiz_BugReport').canAdvance = inputAccepted;
  },
  
  checkDescription: function()
  {
    var desc=document.getElementById("bug-description").value;
    var inputAccepted=(bugReporterApp.setBugDesc(desc));
    document.getElementById('wiz_BugReport').canAdvance = inputAccepted;    
  },

  __dummy: function() {}
};

var bugReporterApp = {
    
  submitter_name: null,
  submitter_email: null,
  bug_desc: null,

  setSubmitter: function(submitter, email)
  {
    submitter.replace(/^\s+/,"");
    submitter.replace(/\s+$/,"");
    email.replace(/^\s+/,"");
    email.replace(/\s+$/,"");
    
    if ( submitter.length > 0 && this._validateEmail(email) )
    {
      this.submitter_name = submitter;
      this.submitter_email = email;
      return true;
    }
    
    return false;
  },

  setBugDesc: function(desc)
  {
    desc.replace(/^\s+/,"");
    desc.replace(/\s+$/,"");
    
    if ( desc.length > 0 )
    {
      this.bug_desc=desc;
      return true;
    }
    
    return false;
  },

  _validateEmail : function(email)
  {
    return email.match(/[^@]+@[^@\.]+\..+/);
  },
  
  __dummy: function() {}
};

