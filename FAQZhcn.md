其他语言 **[English](FAQ.md)**

## 常见问题 ##

### 为什么在从弹出窗口切换回来之后，输入法自动变回了英文？ ###

因为Firefox对于不同的窗口也只有一个输入法状态，所以在切换窗口的时候Firefox同样会丢失输入法状态。

你可以通过启用Firefox的“每个窗口的单独键盘布局（Per Windows Per Keyboard Layout）” 来解决这个问题：
  1. 新建一个标签页，并在地址栏输入 about:config
  1. 在该页面中找到 _`intl.keyboard.per_window_layout`_，将其改变成 _`true`_
  1. 如果在页面当中找不到该值，则在任意的项目上面右击，选择_新建_ -> _布尔值_，在弹出窗口当中输入 _`intl.keyboard.per_window_layout`_，点击确认之后选择_`true`_。
  1. 重启Firefox.

因为Firefox具有这个配置，所以我在设计TabIMSwitch的时候，便没有支持多个窗口的输入法状态切换。请使用该值进行配置。

### 为什么TabIMSwitch会自动添加EN输入法，即使之前只有中文输入法？ ###

因为微软提供的输入法API的bug，导致很多情况下TabIMSwitch无法正确地关闭中文输入法。为了保证在焦点切换到地址栏能够关闭输入法，因此默认情况下TabIMSwitch会自动添加一个英文输入法。添加的英文输入法并不会改变你的系统默认输入法，也不会对正常的输入法切换产生影响。

如果你真的很在意这个添加的输入法，可以通过修改TabIMSwitch的multilingual（多语言）设置来改变TabIMSwitch的默认行为。修改默认行为不保证TabIMSwitch能够正确地切换输入法：
  1. 新建一个标签页，并在地址栏输入 about:config
  1. 在该页面任意的项目上面右击，选择_新建_ -> _布尔值_，在弹出窗口当中输入 _`extensions.tabimswitch.env.multilingual`_，点击确认之后选择_`false`_。
  1. 重启Firefox.

如果你要将其恢复到默认行为，请选中上述项目，并右键点击选择“重置”即可。