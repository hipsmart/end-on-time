# end-on-time
Pebble watch face

Teaching myself to code for the Pebble watch by creating this watchface.

Screenshot of current state, 25 January 2016:

![screenshot](https://raw.githubusercontent.com/hipsmart/end-on-time/master/docs/screenshot2016-01-25.png)

Background:
My wife has an original Pebble. Her desire is for the watch to vibrate on her wrist when 5 minutes are left in class. The classes she teaches happen at different times each day. She accomplishes this by creating events on her Google Calendar--but she has to create special events that start 5 minutes before the end of class, because Google Calendar can only alert you *before or at* the beginning of an event (not relative to when an event ends). She also wants to use the Pebble feature of "calendar aware" Quiet Time, wherein no vibrations are received during an event so phone notifications don't disturb class. So, she creates a calendar event for the class that lasts from the actual start time to 5 minutes before the end time, for the purposes of calendar aware Quiet Time, and a second event that lasts from 5 minutes before a class ends to the end of class, and has an alert set on that event, for the purposes of getting the vibration that class will end soon. This means that she has multiple (2) events for each class (and sets up her calendar to reflect the different days of the week and repeat schedules of classes--all of them requiring two different events to accommodate Quiet Time and the desired alert). A big part of the desired functionality is that she will not have to interact with her watch at any time or need to launch anything--and she can set up her Google Calendar like a normal person with a single event representing a single actual event).

My idea:
I will create a watch face for Pebble that can read a user's Google Calendar. During times when no event is active, the watch will display a normal time. At five minutes before an event starts, the watch face will vibrate and switch into a countdown mode; the number of minutes until class starts will be the primary focus of the face, with the time displayed smaller. Once the event starts, the countdown will reflect the number of minutes until the event ends, and when five minutes remain in the event the watch will vibrate again.
