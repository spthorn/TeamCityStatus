**TeamCityStatus**

Microcontroller-based red/green monitor for Team City projects' status

The project is based on the [Particle Photon[(https://store.particle.io/). Its built-in wifi connects via local WiFi to the TeamCity API and outputs status bits for the various builds.

The prototype phase is an HTML page with webhooks into the Photon to retrieve and display bi-color status svg circle elements, one for each TeamCity project build. Green indicates successful build, red indicates build errors. A quick glance indicates the presence of any failure in the release process.

Next phase will include a large wall-mounted enclosure with indicators easily seen from across the room.