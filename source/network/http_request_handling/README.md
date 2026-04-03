# HTTP Request Handling

This sample demonstrates how to implement asynchronous *HTTP GET* requests to external *REST API* and display the retrieved data in the user interface.

For demonstration, the sample performs two consecutive requests to external weather *API* and displays the results in real time.

<ol>
-**Geocoding** - resolving a location by name using *geocoding-api.open-meteo.com*.
-**Current weather conditions** - retrieving live meteorological data for the selected location using *api.open-meteo.com*.
</ol>
The **![github.com/yhirose/cpp-httplib](https://github.com/yhirose/cpp-httplib)** library is used to perform *HTTP* requests asynchronously. Check the console to view more details from server.

The *JSON* response is processed using the *Json* Class and displayed in the sample *UI*. Additional response details can be viewed in the console output.

You can interactively test the workflow by entering a city name in the *UI*, viewing a list of possible matches, and selecting a specific location. This triggers a request for up-to-date weather data, which is then parsed and displayed in the *UI*.

Asynchronous processing ensures that network operations do not block or degrade the simulation performance.

This sample can serve as a foundation for integrating any external data providers.