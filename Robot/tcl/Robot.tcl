#!bin/sh
#the next line restarts using wish \
exec /usr/local/bin/wish -f "$0" ${1+"$@"}

# robot --
# This script allows the user of the libwww web robot to issue 
# commands to the robot by checking options and clicking buttons.
#
# MK, manoli@w3.org, Manolis Kamvysselis, 96/07/01

#***********************************************************
#Init = initialization and stuff that changes from platform
#Init: initial values for variables
set url "http://www.w3.org/pub/WWW/"

#init: id number for dialog boxes
set id 0 

#Init: Platform and computer dependent variables
proc InitPath {} {
	global path webbot weblog webout webrules path source editor \
			webbot-path weblog-path webout-path webrules-path
	if [file exists webprefs] {
		LoadPreferences
	} else {
		#prefs file does not exist: prompt user
		Dialog "File webprefs not found. \n \
			Would you like to create a new file \
			in the current directory?" \
			{create set cancel} \
			{"Create File with Default Preferences" "Set Default Preferences" "Cancel"} \
			{{SetDefaultPreferences; SavePreferences} SetDefaultPreferences ""}
	}
}

#procedure to create a dialog box displaying a message,
#defaults to ok and cancel, spelling Ok and Cancel and doing nothing
proc Dialog {message {buttons {ok cancel}} {names {"ok" "cancel"}} {commands {"" ""}}} {
	global id
	set id [expr $id + 1]
	if {"winfo exists .dialog$id" == 1} {
		wm withdraw .dialog$id
	}
	toplevel .dialog$id
	wm title .dialog$id "Dialog Box"
	message .dialog$id.msg -aspect 1000 -text $message
	pack .dialog$id.msg -side top
	frame .dialog$id.buttons
	foreach button $buttons {
		set name [lindex $names [lsearch -exact $buttons $button]]
		set command [lindex $commands [lsearch -exact $buttons $button]]
		button .dialog$id.buttons.$button -text $name -command "wm withdraw .dialog$id; $command"
		pack .dialog$id.buttons.$button -side left
	}
	pack .dialog$id.buttons -side bottom
}

#Window: name the window and set up bindings
proc SetWindow {} {
	wm title . "W3C Web Robot user interface"	
	
	#key bindings equivalents to buttons
	bind . <Return> Start
	bind . <Control-x> exit
	bind . <Control-q> exit	
	bind . <Control-p> AskPreferences
}

#***********************************************************
#Menu: preferences for paths, and stuff all in a menu
proc SetMenus {} {
	global webbot source
	#creates three menubuttons File, Edit, About
	frame .menu
	foreach which {file edit help} {
		set Capitalized ""
		append Capitalized [string toupper [string index $which 0]] \
			[string range $which 1 end] 
		menubutton .menu.$which -text $Capitalized -menu .menu.$which.menu
		pack .menu.$which -side left
	}
	pack .menu -side top -fill x
	
	#Menu: Robot: files to be used for output, log, and executable of the robot
	set mfile [menu .menu.file.menu]
	$mfile add command -label Run -command Start
	$mfile add command -label "Edit Source" -command {exec $editor $source}
	$mfile add separator
	$mfile add command -label "Load Prefs..." -command {
		Dialog "Load Preferences: This will discard the current Preferences" \
		{ok set cancel} \
		{"Go ahead" "View Current Preferences" "Abort the Mission"} \
		{LoadPreferences AskPreferences ""}
	}
	$mfile add command -label "Save Prefs..." -command {	
		Dialog "Save Preferences: This will discard the Preferences Saved Previously" \
		{ok set cancel} \
		{"Go ahead" "View Current Preferences" "Abort the Mission"} \
		{SavePreferences AskPreferences ""}
	}
	$mfile add separator
	$mfile add command -label Preferences... -command AskPreferences
	$mfile add separator
	$mfile add command -label Quit -command exit
	
	#Menu: edit copy paste and stuff
	set medit [menu .menu.edit.menu]
	$medit add command -label "Copy and Paste Url" -command CopyAndPasteUrl
	$medit add command -label "Follow URL" -command FollowUrl
	
	#Menu: about: version of the robot, and about the program
	set mhelp [menu .menu.help.menu]
	set blablahelp {"		To fetch links from a URL, enter the url and click webbot. 		

		To follow a URL returned, double click in the log, on the line of the 		

		URL that you want to follow. 		

		To simply copy a URL to the URL entry, so you can edit it afterwards, 		

		click the left then the right button on the corresponding line of the log. 		"}
	$mhelp add command -label "Using" -command "Dialog $blablahelp ok Cool {}"
		
	$mhelp add command -label "About the Robot" -command ShowVersion
	set blablatext {"		Welcome to the W3C mini robot user interface. 		

		The selections that you make below, or in the preferences, are 		

		translated and passed on to the W3C mini robot, part of the 		

		w3c test library.		


		Have fun!!"}
	$mhelp add command -label "About this Program" -command "Dialog $blablatext ok Thanx {}"
}
		
#***********************************************************
#Prefs: window to prompt for preferences
proc AskPreferences {} {
	global editor source path webbot weblog webrules
	toplevel .prefs
	wm title .prefs "W3C WebRobot User Preferences"

	#Editor: Which editor to use to edit the source
	frame .prefs.editor
	label .prefs.editor.l -text Editor:
	entry .prefs.editor.path -textvariable editor
	pack .prefs.editor.l -side left
	pack .prefs.editor.path -side left -expand true -fill x
	pack .prefs.editor -side top -fill x

	#***********************************************************
	#Edit: where to find the source of robot.tcl and editor to launch
	frame .prefs.edit
	label .prefs.edit.l -text Source:
	entry .prefs.edit.source -textvariable source 
	button .prefs.edit.edit -text Edit -command {exec $editor $source}
	pack .prefs.edit.l -side left
	pack .prefs.edit.source -side left -fill x -expand true
	pack .prefs.edit.edit -side right
	pack .prefs.edit -side top -fill x

	#***********************************************************
	#Path: Choose the common path of webbot weblog and webrules
	frame .prefs.path	
	checkbutton .prefs.path.common -text Common -variable common \
		-onvalue "Same" -offvalue "Different" -command {SetPath$common}
	.prefs.path.common select
	label .prefs.path.l -text Path:
	entry .prefs.path.webbot -width 50 -relief sunken -textvariable path
	bind .prefs.path.webbot <KeyRelease> {SetPath$common}
	pack .prefs.path.common .prefs.path.l -side left
	pack .prefs.path.webbot -side left -expand true
	pack .prefs.path -side top -fill x

	#***********************************************************
	#SetPath*: procedures to set the path
	proc SetPathSame {} {
		global path
		foreach what {bot log rules out} {
			global web$what web$what-path
			set web$what ""
			append web$what $path [set web$what-path]
		}
	}
	proc SetPathDifferent {} {
		foreach what {bot log rules out} {
			global web$what web$what-path
			set web$what [set web$what-path]
		}
	}

	#***********************************************************
	#Log: choose the log file
	array set Text {bot Webbot: log "Log File:" rules "Rules File:" \
				out "Output File:"}
	array set Value {bot webbot.exe log www-log rules www-rule out www-out}
	foreach what {bot log rules out} {
		global web$what web$what-path
		frame .prefs.$what
		set web$what-path $Value($what) 
		label .prefs.$what.l -text $Text($what)
		entry .prefs.$what.path -textvariable web$what-path
		bind .prefs.$what.path <KeyRelease> {
			global what
			SetPath$common
		}
		label .prefs.$what.show -textvariable web$what -width 30
		pack .prefs.$what.l -side left
		pack .prefs.$what.path -side left -expand true -fill x
		pack .prefs.$what.show -side right -expand true
		pack .prefs.$what -side top -fill x
	}
	
	#***********************************************************
	#Buttons: What to do once the preferences are set
	frame .prefs.buttons
	button .prefs.buttons.set -text "Set" -command ClosePrefsWindow
	button .prefs.buttons.save -text "Save" -command "SavePreferences; ClosePrefsWindow"
	button .prefs.buttons.load -text "Load" -command "LoadPreferences; ClosePrefsWindow"
	button .prefs.buttons.edit -text "Edit" -command "$editor ${path}Webprefs"
	foreach which {set save load edit} {
		pack .prefs.buttons.$which -side left
	}
	pack .prefs.buttons -side top

	proc ClosePrefsWindow {} {
		wm withdraw .prefs
	}
}

#prefs: initialize preferences, without saving them to a file
proc SetDefaultPreferences {} {
	global path source editor webbot-path weblog-path webrules-path webout-path
	set source ""
	set path [pwd]\\
	lappend source $path Robot.tcl
	set editor "C:\\Program Files\\Accessories\\WordPad.exe"
	set webbot-path webbot.exe
	set weblog-path www-log
	set webrules-path webrules
	set webout-path www-out
	foreach what {bot log out rules} {
		global path web$what web$what-path
		set web$what ""
		append web$what $path [set web$what-path]
	}
}

#prefs: Load preferences from webprefs
proc LoadPreferences {} {
	global editor source path webbot weblog webrules
	Status Loading Preferences...
	if [file exists webprefs] {
		#the file webprefs exists: read the preferences
		if [catch {open webprefs r} file] {
			#file can't be read
			Dialog "File Unreadable" ok "OK" ""
		} else {
			#file can be read
			while {! [eof $file]} {
				#still lines to read
				gets $file line
				if {[string compare # [string index $line 0]] == 0} {
					#ignores commented lines
				} else {
					#assuming line of the form variable:value
					set pair [split $line :]
					set variable [lindex $pair 0]
					set value [join [lrange $pair 1 [llength $pair]] :]
					global $variable
					set $variable $value
					Log "setting $variable to $value"
				} 
			}
			close $file
			TraceToButtons
		}
	} else {
		#file webprefs does not exist
		Dialog "		File WebPrefs does not Exist.		
		Create a file WebPrefs in the current directory?		
		Current Directory: [pwd]		"\
		{create default cancel} \
		{"Create File with Default Preferences" "Set default Preferences" "Cancel"} \
		{{SetDefaultPreferences; SavePreferences} SetDefaultPreferences ""}
	}
	Status Done
}

#prefs: create the prefs file to hold the preferences
proc SavePreferences {} {
	set file [open webprefs w 0600]
	puts $file "#This file is created by webbot.tcl"
	puts $file "#It contains the preferences for running the w3c mini robot"
	foreach variable {path source editor webbot webbot-path weblog weblog-path webout webout-path \
		webrules webrules-path trace img ss url} {
		global $variable
		puts $file $variable:[set $variable]
		Log Writing:$variable:[set $variable]
	}
	close $file		
}

#***********************************************************
#nav: Navigation method, and depth
proc SetNav {} {
	frame .nav
	set nav dumb
	label .nav.l1 -text "Navigation method:" -relief ridge
	pack .nav.l1 -side left
	frame .nav.choice
	foreach choice {dumb parallel deep strategic} {
		radiobutton .nav.choice.$choice -text $choice -value $choice -variable nav
		pack .nav.choice.$choice -side left
	}
	pack .nav.choice -side left
	
	#nav: links: Number of links to follow
	frame .nav.links
	label .nav.links.l -text "Depth:" -relief ridge
	set links 0
	set link ""
	entry .nav.links.entry -textvariable links -width 3 -relief flat
	bind .nav.links.entry <KeyRelease> SetLink
	proc SetLink {} {
		global links link
		if {[string compare $links ""] == 0} {
			set link ""
		} elseif {$links != 0} {
			set link ""
			append link "-link " $links
		} else {
			set link ""
		}
	} 
	label .nav.links.show -textvariable link
	foreach packing {l entry show} {
		pack .nav.links.$packing -side left
	}
	pack .nav.links -side left
	pack .nav -side top -fill x
}
	
#***********************************************************
#Trace: Tracing information on the robot
proc SetTrace {} {
	global trace traced
	set trace ""
	frame .trace
	
	#Trace: title, variable, and button for debugging
	frame .trace.title
	label .trace.title.l -text "Trace messages relevant to:" -relief ridge
	button .trace.title.all -text Everything -command TraceAll
	button .trace.title.none -text Nothing -command TraceNone
	pack .trace.title.l .trace.title.all .trace.title.none -side left
	label	.trace.title.display -textvariable trace -justify left -width 10
	foreach whichbutton {l all none display} {
		pack .trace.title.$whichbutton -side left
	}
	
	#***********************************************************
	#Info: Retrieving Information
	frame .trace.title.info
	checkbutton .trace.title.info.version -text Version -command ShowVersion
	
	#Info: Checking the images of the documents
	checkbutton .trace.title.info.img -text "Images" \
		-variable img -onvalue "-img" -offvalue ""
		
	#Info: Date and Time of the job
	checkbutton .trace.title.info.ss -text "Time Log" \
		-variable ss -onvalue "-ss" -offvalue ""
	
	#Info: Packing the buttons
	foreach packing {version img ss} {
		pack .trace.title.info.$packing -side left
	}
	pack .trace.title.info -side left
	pack .trace.title -side top -fill x
	
	#Trace: buttons to choose tracing options
	frame .trace.buttons

	#matching up the names and letters
	array set tracing {a "anchors" b "bindings to local files"
		c "cache" g "SGML" p "protocol" s "sgml/html" t "threads" u "url"}
	foreach letter {a b c g p s t u} {
		checkbutton .trace.buttons.$letter -text $tracing($letter) \
			-variable traced($letter) -command ButtonsToTrace
		pack .trace.buttons.$letter -side left -anchor w
	}
	pack .trace.buttons -side top -fill x -expand true
	pack .trace -side top -fill x
	
	#Trace: tracing everything or nothing
	proc TraceAll {} {
		global traced ss img
		foreach letter {a b c g p s t u} {
			set traced($letter) 1
		}
		ButtonsToTrace
		set img "-img"
		set ss "-ss"
	}
	proc TraceNone {} {
		global traced img ss
		foreach letter {a b c g p s t u} {
			set traced($letter) 0
		}
		ButtonsToTrace
		set img ""
		set ss ""	
	}
}
	
#Trace: Showing the Version
proc ShowVersion {} {
	global webbot
	catch {exec $webbot "-version"} results
	Log $results
}

#Trace: setting up the buttons from the variable trace
proc TraceToButtons {} {
	global trace traced
	if {[string compare $trace "-v"] == 0} {set trace -vabcgpstu}
	foreach letter {a b c g p s t u} {
		global traced($letter)
		Log "traced($letter) = [set traced($letter)]"
		if "[string match *$letter* $trace] == 1" {
			set traced($letter) 1
		} else {
			set traced($letter) 0
		}
	}
	if {[string compare $trace "-vabcgpstu"] == 0} {set trace -v}
}

#Trace: calculating what to trace from the user's selections
proc ButtonsToTrace {} {
	global trace traced
	set trace "-v"
	foreach letter {a b c g p s t u} {
		if {$traced($letter)} {
			append trace $letter
		}
	}
	if {[string compare $trace "-v"] == 0} {
		set trace ""
	} elseif {[string compare $trace "-vabcgpstu"] == 0} {
		set trace "-v"
	}
}
	
#***********************************************************
#Webbot: Launch the robot and Url to start from
proc SetWebbot {} {
	frame .webbot
	label .webbot.l -text "URL to start from:"
	entry .webbot.url -textvariable url -foreground red
	pack .webbot.l -side left 
	pack .webbot.url -side left -fill x -expand true
	button .webbot.start -text Webbot -command Start
	button .webbot.exit -text Exit -command exit
	pack .webbot.exit .webbot.start -side right
	pack .webbot -side top -fill x
}

#***********************************************************
#Log: Creating a scrollable window to log the output
proc SetLog {} {
	global log
	frame .log
	set log [text .log.log -width 75 -height 10 -wrap none -borderwidth 2 \
		-relief raised -setgrid true -yscrollcommand {.log.scrolly set} \
		-xscrollcommand {.log.scrollx set}]
	scrollbar .log.scrolly -orient vertical -command {.log.log yview}
	scrollbar .log.scrollx -orient horizontal -command {.log.log xview}
	pack .log.scrolly -side right -fill y
	pack .log.scrollx -side bottom -fill x
	pack .log.log -side left -fill both -expand true
	pack .log -side top -fill both -expand true
	
	#Log: Key Bindings
	bind .log.log <Control-f> FollowUrl
	bind .log.log <Control-u> CopyAndPasteUrl
	bind .log.log <Double-Button-1> FollowUrl
	bind .log.log <Button-2> CopyAndPasteUrl
	bind .log.log <Button-3> CopyAndPasteUrl
}

#Log: Procedure to log results in the log window
proc Log {string} {
	global log
	$log insert end $string\n
	$log see end
}

#Log: procedures to follow URLs from the log
proc FollowUrl {} {
	CopyAndPasteUrl
	Start
}

proc CopyAndPasteUrl {} {
	global log url
	set url [lindex [split [$log get "insert linestart" "insert lineend"] {`'}] 1]
	$log tag add fetched "insert linestart" "insert lineend"
	$log tag configure fetched -foreground red
}

#***********************************************************
#Status: Looking at what's going on
proc SetStatus {} {
	set status Ready
	frame .status
	label .status.l -text "Status:"
	label .status.status -textvariable status -relief sunken -anchor w
	pack .status.l -side left
	pack .status.status -side left -fill x -expand true
	pack .status -side bottom -fill x
}

#Status: procedure status to be called with any number of arguments
proc Status {args} {
	global status
	set status $args
}

#***********************************************************
#Start running the Robot
proc Start {} {
	global webbot img link ss trace url
	Status Running the robot...
	set Space(start) "********************************* $url"
	set Space(end) {}
	Log $Space(start)
        Log "Calling webbot with $webbot $img $link $ss $trace $url"
	catch {eval exec $webbot $img $link $ss $trace $url} results
	Log $results
	Log $Space(end)
	Status Done
}

#***********************************************************
#Main: the main program that calls all of the previous functions
SetWindow
SetMenus
SetNav
SetTrace
SetWebbot
SetLog
SetStatus
SetDefaultPreferences
InitPath