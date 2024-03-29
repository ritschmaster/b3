# Compatibility list

## Config: functions

The following functions in the config file are fully supported:

* bindsym \<KEY BINDING\> exec
* bindsym \<KEY BINDING\> exec --no-startup-id
* bindsym \<KEY BINDING\> kill
* bindsym \<KEY BINDING\> fullscreen toggle
* bindsym \<KEY BINDING\> floating toggle
* bindsym \<KEY BINDING\> focus up
* bindsym \<KEY BINDING\> focus down
* bindsym \<KEY BINDING\> focus left
* bindsym \<KEY BINDING\> focus right
* bindsym \<KEY BINDING\> focus right
* bindsym \<KEY BINDING\> move up
* bindsym \<KEY BINDING\> move down
* bindsym \<KEY BINDING\> move left
* bindsym \<KEY BINDING\> move right
* bindsym \<KEY BINDING\> move right
* bindsym \<KEY BINDING\> focus output up
* bindsym \<KEY BINDING\> focus output down
* bindsym \<KEY BINDING\> focus output left
* bindsym \<KEY BINDING\> focus output right
* bindsym \<KEY BINDING\> focus output right
* bindsym \<KEY BINDING\> move container to output up
* bindsym \<KEY BINDING\> move container to output down
* bindsym \<KEY BINDING\> move container to output left
* bindsym \<KEY BINDING\> move container to output right
* bindsym \<KEY BINDING\> move container to output right
* bindsym \<KEY BINDING\> move workspace to output left
* bindsym \<KEY BINDING\> move workspace to output down
* bindsym \<KEY BINDING\> move workspace to output up
* bindsym \<KEY BINDING\> move workspace to output right
* bindsym \<KEY BINDING\> move workspace to output right
* bindsym \<KEY BINDING\> split h
* bindsym \<KEY BINDING\> split v
* bindsym \<KEY BINDING\> workspace \<WORKSPACE NAME\>
* bindsym \<KEY BINDING\> move container to workspace \<WORKSPACE NAME\>
* for_window [title="\<REGEX\>"] floating enable
  * Also supports the special value `__focused__`
* for_window [class="\<REGEX\>"] floating enable
  * Also supports the special value `__focused__`

## Config: key bindings

The following keys are currently supported to be used within a key binding:

* `a` to `z`
* `0` to `9`
* `F1` to `F12`
* The super key (`Mod4`)
* The alt key (`Mod1`)
* Space
* Control
* Shift
* Return
