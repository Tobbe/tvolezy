tVolEzy - The easy to use volume changing module written by Tobbe
=================================================================

This module is supposed to control the main system volume using simple bang
commands. It's for those of you who just want to control your volume and
don't need to display a slider or anything like that.


Loading the module
------------------

There are no settings to write in step.rc (or any of the included files, like
To load the module, use either

    LoadModule "$ModulesDir$tVolEzy-0.1.dll"

or the newer NetLoadModule

    *NetLoadModule tVolEzy-0.1


.rc settings and commands
-------------------------

tVolEzyShowErrors true
    Setting this to 'false' will disable all error message boxes.
    Default is 'true'.

tVolEzyUnmuteOnVolUp true
    Makes tVolEzy unmute the sound when you call !tVolEzyUp
    Default is 'true'

tVolEzyUnmuteOnVolDown false
    Makes tVolEzy unmute the sound when you call !tVolEzyDown
    Default is 'false'


!bang commands
--------------

!tVolEzyUp 1
    Increases the volume the specified amount of steps. Minimum volume is 0 and
    full volume is 65535.
    The parameter is optional and defaults to 1 if not specified.

!tVolEzyDown 1
    Decreases the volume the specified amount of steps. Minimum volume is 0 and
    full volume is 65535.
    The parameter is optional and defaults to 1 if not specified.

!tVolEzyToggleMute
    Use this !bang to mute or unmute the sound. !tVolEzyToggleMute mutes the
    system sound when it isn't muted and unmutes it when it is muted.


Version history
---------------

Tobbe 2009-09-13 tVolEzy 0.1
    Released the first version, 0.1.
