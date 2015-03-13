# 已知问题 #
其他语言 **[English](KnownIssues.md)**

## 所有版本 ##

### 在Windows XP和Vista上面无法保存标签页的输入法状态 ###

**行为：** 在某些系统上，有时候标签页的输入法状态无法保存。比如，你在一个标签页当中把输入法切换成微软拼音，然后切换到另外的标签页。这个时候输入法能够正常的关闭，但是当切换回刚才的标签页的时候，输入法并没有切换回微软拼音。

**解决方案：** 在Windows XP当中，请为Firefox关闭高级文字服务。尝试按照下面的步骤操作：
  1. 在控制面板当中选择“区域和语言选项”；
  1. 选择“语言”标签，并单击“详细信息”按钮；
  1. 在打开的“文字服务和输入语言”窗口当中，**取消选中** “将高级文字服务支持应用于所有程序”前面的复选框。

![http://tabimswitch.googlecode.com/files/deselect-advanced-text-services-zhcn.png](http://tabimswitch.googlecode.com/files/deselect-advanced-text-services-zhcn.png)

