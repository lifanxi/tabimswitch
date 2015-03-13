Other Language **[简体中文](FAQZhcn.md)**

## Frequently Asked Questions ##

### Why the input method changes back to English after I navigate back from a pop up window? ###

You may try to enable Firefox's "Per Windows Per Keyboard Layout" setting.
  1. Open a new tab, and type about:config in the address bar.
  1. From the page, find the item _`intl.keyboard.per_window_layout`_, and change the value to _`true`_
  1. If you may not find the item, right-click on any item, and select _New_ -> _Boolean_, from the popup window, type _`intl.keyboard.per_window_layout`_, click OK, and select value as _`true`_.
  1. Restart Firefox.

Because Firefox has this setting, so when I design TabIMSwitch, I did not support multiple wIndow IM state switch, because people can use this setting to workaround.

### Why TabIMSwitch adds English input method automatically? ###

Because a bug in Microsoft API, TabIMSwitch is not able to close input method (especially some Chinese Pinyin input methods) correctly under some conditions. To make sure when user focuses on the address bar, the input method is closed, TabIMSwitch will add an English input method to workaround the API bug. The added input method will neither impact your system default input method, nor impact your normal input method switch.

If you really cares about the input method added by TabIMSwitch, you can modify TabIMSwitch's default behavior with TabIMSwitch's multilingual setting. Changing the default setting does not guarantee TabIMSwitch working as expected.
  1. Open a new tab, and type about:config in the address bar.
  1. Right-click on any item, and select _New_ -> _Boolean_, from the popup window, type _`extensions.tabimswitch.env.multilingual`_, click OK, and select value as _`false`_.
  1. Restart Firefox.

If you want to revert to the default behavior, please select the above item, right click on it and select "reset".