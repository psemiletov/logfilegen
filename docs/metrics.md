## Metrics

logfilegen supports the metrics output in Prometheus/OpenMetrics format.

To request current metrics, just do the HTTP GET request to ```localhost:8080/metrics```

To open the self-hosted logfilegen page with metrics, go to ```localhost:8080```

To redefine default port use ```--port=number``` logfilegen option.

To set the self-hosting page auto-update interval use ```--poll=seconds``` option (default: 15, in seconds)

Please note that per-second metrics are average values, calculated by the division of total value and seconds pass.
