# Configuration over WiFi
1. Connect to `skelenet` network.
2. Go to `skeleton.local` if your device supports mDNS or `192.168.4.1` otherwise.

## HTTP API
<table>
<thead>
<tr>
<th>Endpoint</th>
<th>Parameter</th>
<th>Min value</th>
<th>Max value</th>
<th>Action</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan=4><code>/update</code></td>
<td>baseDelay</td>
<td>0</td>
<td>3 600 000ms (1hr)</td>
<td>Constant delay base. Updating resets sleep.</td>
</tr>
<tr>
<td>randDelay</td>
<td>0</td>
<td>3 600 000ms (1hr)</td>
<td>Random delay upper bound. Total sleep delay is calculated as baseDelay + random(0, randDelay). Updating resets sleep.</td>
</tr>
<tr>
<td>upDuration</td>
<td>1000ms (1s)</td>
<td>5000ms (5s)</td>
<td>Duration of travel from lying to sitting position.</td>
</tr>
<tr>
<td>downDuration</td>
<td>1000ms (1s)</td>
<td>5000ms (5s)</td>
<td>Duration of travel from sitting to lying position.</td>
</tr>
<tr>
<td><code>/raise</code></td>
<td>-</td>
<td>-</td>
<td>-</td>
<td>Restarts skeleton loop immediately.</td>
</tr>
</tbody>
</table>
