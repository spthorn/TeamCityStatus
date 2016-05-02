**Overview**

For instructions on how to write code for the Photon, there is ample documentation
online.

**Accessing the TeamCity API**

Authentication: insert the user:pass in front of the URL
and be sure to include httpAuth prior to the /app:
```
http://username:password@build.renovolive.com/httpAuth/app/rest/builds
```
Do this once and you're in for the remainder of the session, so all you
need from that point on is this:
```
http://build.renovolive.com/httpAuth/app/rest/
```

Retrieve latest build status for a given project/buildType. First, find all the
build types for a given project.
```
http://.../rest/projects/id:RenovoLiveLegacy
```
Within each project, wewant to look at all build types (which is an ID string 
unique across all projects). Iterate through all
projects (/rest/projects/) and build the top-level project hierarchy - note the
"parentProjectId" member in each project XML record. 
Then, get the list of build types for each project.

Once we have this list of build types, get the latest builds for them:
```
http://.../rest/builds/?locator=buildType:bt14,count:5
```
(you could really just use count:1)
The XML returned includes a status member with "SUCCESS" or "FAILURE".

Sample output:
```
<builds count="1" href="/httpAuth/app/rest/builds/?locator=buildType:bt14,count:1" nextHref="/httpAuth/app/rest/builds/?locator=buildType:bt14,count:1,start:1">
  <build id="58502" buildTypeId="bt14" number="1.0.1425" status="SUCCESS" state="finished" branchName="master" defaultBranch="true" href="/httpAuth/app/rest/builds/id:58502" webUrl="http://build.renovolive.com/viewLog.html?buildId=58502&buildTypeId=bt14"/>
</builds>
```

One alternative to using the HttpClient library in the Photon is to make API calls
from an external web page, and have the Photon just function as a state machine.
See https://community.particle.io/t/continue-loop-while-waiting-for-post-to-finish/18901/3 
for a suggestion on how to implement. It requires a publically accessible web page,
though, fwiw. That may be a good thing, however, so it could be used as a Photon
"dashboard". OTOH, it kind of defeats the swiss-army-knife function of the Photon.

**Performance**

Currently, it takes 21 seconds to retrieve information (Projects, BuildTypes, Builds)
from the TeamCity API.

**Libraries**

I use two libraries, HttpClient and SparkJson. The former is used to communicate with the TeamCity API. Unfortunately, as is, it is not
workable for this API, which returns too much data. I needed to pull in the code and increase the buffer size from 1K to 8K. 

The SparkJson library I use as is, for parsing the JSON data from the API. It's a little hard to understand at first. These links
helped a bit:
* See docs at https://github.com/menan/SparkJson
* See https://github.com/bblanchon/ArduinoJson/wiki/Avoiding-pitfalls
* https://community.particle.io/t/getting-an-array-of-data-from-spark/11376/9


**Troubleshooting**

Factory reset:
* Begin holding the MODE button down.
* While holding the MODE button down, tap the RESET button briefly.
* After 3 seconds the core will begin blinking yellow, KEEP HOLDING the MODE button.
* After 10 seconds the core will begin blinking white. When this happens the factory reset process has begun. Let go of the MODE button.

When it stops blinking white, it should begin blinking blue, indicating that it is listening for WiFi credentials.
Via the CLI: `particle setup wifi`. I'm using
the network "Renovo24" (Brandon set it up - to remove the 5GHz band), WPA2, [our std wifi password].

Serial debugging:

Windows should automatically connect the USB output of the Photon to a serial port. You may or may not
need to update the driver - see the Particle docs for the driver.

Open up your command line (cmd or git bash):
* Log in by running `particle login`
* Run `particle serial monitor`

You'll get booted out of the serial monitor each time you flash or reboot your Photon.










