## Metrics

logfilegen supports the metrics output in Prometheus/OpenMetrics format. By default, logfilegen uses built-in server code. but you can enable [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp) library at the Cmake stage (see [Installation guide](inst.md)). At this point of the development, the build-in server code is more mature, so use it.

To request current metrics, just do the HTTP GET request to ```localhost:8080/metrics```

To open the self-hosted logfilegen page with metrics, go to ```localhost:8080```

To redefine default port use ```--port=number``` logfilegen option.

To set the self-hosting page auto-update interval use ```--poll=seconds``` option (default: 15, in seconds)

