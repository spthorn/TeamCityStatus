**TeamCityStatus

Microcontroller-based red/green monitor for Team City projects' status

The project is based on the Particle Photon. Its built-in wifi connects via local WiFi to the TeamCity API and outputs status bits for the various builds.

The prototype phase includes bi-color status LEDs, one for each TeamCity project build. Green indicates successful build, red indicates build errors. The prototype will include a matrix of LEDs - one column per project (Portal, Legacy, RL2...) and one row per build (Compile, Dev deploy, Smoke tests, etc). A quick glance indicates the presence of any failure in the release process.

Next phase will include a larger enclosure with larger indicators.