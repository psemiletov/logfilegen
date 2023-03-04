#! /bin/bash

# Variables
base_dir="${base_dir}"
instance_type="${instance_type}"
data_volume_size="${data_volume_size}"
data_volume_type="${data_volume_type}"
data_volume_iops="${data_volume_iops}"
data_volume_throughput="${data_volume_throughput}"
benchmark_count="${benchmark_count}"
benchmark_duration="${benchmark_duration}"
benchmark_auto="${benchmark_auto}"
results_bucket="${results_bucket}"
results_bucket_key="${results_bucket_key}"


# Wait and relax
sleep 60


# Benchmarking
export LFG_DURATION=$benchmark_duration
export LFG_MODE="nginx"
export LFG_LOGFILE="$base_dir/out.log"
export LFG_LOGSIZE="1G"
export LFG_RESULTS="stdout"
export LFG_RESULTS_TEMPLATE="@date - @duration - @mode/@template - @size_generated - @lines_generated - @performance_bps - @performance_lps"
entry="$instance_type $data_volume_size $data_volume_type $data_volume_iops $data_volume_throughput"
log=$base_dir/$(sed 's/ /-/g' <<< "$entry").txt

for i in $(seq $benchmark_count); do
  results=$(logfilegen)
  echo "$entry - $results" >> "$log"
  rm -f $LFG_LOGFILE*
  sleep 5
done


# Upload benchmarking results to S3
if [[ $results_bucket != "null" ]]; then
  # Get credentials
  token=$(curl -X PUT -s "http://169.254.169.254/latest/api/token" -H "X-aws-ec2-metadata-token-ttl-seconds: 21600")
  role=$(curl -s -H "X-aws-ec2-metadata-token: $token" http://169.254.169.254/latest/meta-data/iam/security-credentials)
  credentials=$(curl -s -H "X-aws-ec2-metadata-token: $token" http://169.254.169.254/latest/meta-data/iam/security-credentials/$role)

  # Use credentials
  export AWS_ACCESS_KEY_ID=$(jq -r '.AccessKeyId' <<<$credentials)
  export AWS_SECRET_ACCESS_KEY=$(jq -r '.SecretAccessKey' <<<$credentials)
  export AWS_SESSION_TOKEN=$(jq -r '.Token' <<<$credentials)

  # Uplaod results to S3
  aws s3 cp "$log" "s3://$results_bucket/$results_bucket_key/$(basename $log)"
fi


# Shutdown the node
[[ $benchmark_auto == "true" ]] && halt -p
