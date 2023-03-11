# Performance testing

 1. [Description](#description)
 2. [Methodology](#methodology)
 3. [Results](#results)
 4. [Conclusions](#conclusions)


## Description

 Log generation sometimes may be used for load testing and for that it is suitable to have some data about generation performance.

 Which resources we may use to achieve required generation performance or a balance between the cost and performance.

 The scope of experiment
 1. Use AWS instances as a stand which may be easily reproduced
 2. Use default nginx log format
 3. Calculate generation rate in lines per second
 4. Calculate generation throughput in megabytes per second
 5. Calculate costs required to achieve one million lines per second


## Methodology

 1. Create an AWS EC2 instance.
 2. Attach to the instance a separate EBS volume, with the required performance.
 3. For [storage optimised instances](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/storage-optimized-instances.html), we will use NVMe SSD instance store volumes instead of EBS.
 4. Use latest logfilegen version.
 5. Run test 3 times for 5 minutes and generate logs on a separate volume.
 6. Record lines generation and throughput performance.
 7. Calculate costs to achieve 1 million lines per second on the current instance type, excluding the storage use cost.

  <details><summary> Calculation example </summary>

  ```bash
  # Variables
  benchmark_count=3
  report="results.log"
  export LFG_DURATION=300
  export LFG_MODE="nginx"
  export LFG_LOGFILE="out.log"
  export LFG_LOGSIZE="1G"
  export LFG_RESULTS="stdout"
  export LFG_RESULTS_TEMPLATE="@date - @duration - @mode/@template - @size_generated - @lines_generated - @performance_bps - @performance_lps"

  # Benchmarking
  for i in $(seq $benchmark_count); do
    results=$(logfilegen)
    echo "$results" >> "$report"
    rm -f $LFG_LOGFILE*
    sleep 10
  done

  # Analysis
  num=$(cat $report | wc -l)
  rate=$(awk -F ' - ' '{n += $NF}; END{print int (n / '$num')}' $report)
  throughput=$(awk -F ' - ' '{n += $(NF-1)}; END{print int (n / '$num' / 1024 / 1024)}' $report)
  echo
  echo "$rate lps - $throughput MB/s ($report)"
  echo
  ```
  ```
   228721 lps - 21 MB/s (results.log)
  ```

  [t3.micro](https://aws.amazon.com/ec2/instance-types/t3/) On-Demand Price/hr is `$0.0104`

  `1000000l / 228721lps * 0.0104$/h = 0.0455$/h`

  </details>

 We may use [Terraform code for AWS](aws/README.md) to perform manual or automated performance testing using [GitHub Actions](https://github.com/features/actions).


## Results

<details><summary>Instance types</summary>

 - [Instance Types](https://aws.amazon.com/ec2/instance-types/)
 - [Instance types](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/instance-types.html)

| Instance                                                                     | Type              | Processors                                             |
| ---------------------------------------------------------------------------- | ----------------- | -------------------------------------------------------|
| [T3 Instances](https://aws.amazon.com/ec2/instance-types/t3/)                | General Purpose   | Intel Xeon Platinum 8000, 1/2nd gen                    |
| [T4g Instances](https://aws.amazon.com/ec2/instance-types/t4/)               |                   | AWS Graviton2                                          |
| [M6i Instances](https://aws.amazon.com/ec2/instance-types/m6i/)              |                   | Intel Xeon Scalable Ice Lake, 3nd gen                  |
| [M6a Instances](https://aws.amazon.com/ec2/instance-types/m6a/)              |                   | AMD EPYC Milan, 3nd gen                                |
| [M6g Instances](https://aws.amazon.com/ec2/instance-types/m6g/)              |                   | AWS Graviton2                                          |
| [M7g Instances](https://aws.amazon.com/ec2/instance-types/m7g/)              |                   | AWS Graviton3                                          |
| [C6i Instances](https://aws.amazon.com/ec2/instance-types/c6i/)              | Compute Optimized | Intel Xeon Scalable Ice Lake, 3rd gen                  |
| [C6a Instances](https://aws.amazon.com/ec2/instance-types/c6a/)              |                   | AMD EPYC Milan, 3rd gen                                |
| [C6g Instances](https://aws.amazon.com/ec2/instance-types/c6g/)              |                   | AWS Graviton2                                          |
| [C7g Instances](https://aws.amazon.com/ec2/instance-types/c7g/)              |                   | AWS Graviton3                                          |
| [R6i Instances](https://aws.amazon.com/ec2/instance-types/r6i/)              | Memory Optimized  | Intel Xeon Scalable Ice Lake, 3nd gen                  |
| [R6a Instances](https://aws.amazon.com/ec2/instance-types/r6a/)              |                   | AMD EPYC Milan, 3nd gen                                |
| [R6g Instances](https://aws.amazon.com/ec2/instance-types/r6g/)              |                   | AWS Graviton2                                          |
| [R7g Instances](https://aws.amazon.com/ec2/instance-types/r7g/)              |                   | AWS Graviton3                                          |
| [I3 Instances](https://aws.amazon.com/ec2/instance-types/i3/)                | Storage optimized | Intel Xeon Broadwell E5-2686 v4                        |
| [I4i Instances](https://aws.amazon.com/ec2/instance-types/i4i/)              |                   | Intel Xeon Scalable Ice Lake, 3rd gen                  |

</details>

| Instance        | Rate, lps | Throughput, MB/s | Cost, $/h/1m lps | Architecture | vCPU | Memory, GiB | Data Disk                  |
| --------------- | --------- | ---------------- | ---------------- | ------------ | ---- | ----------- | -------------------------- |
| **t2.micro**    |  `55,624` |  `5`             | `0.2085`         | x86_64 Intel | 1    | 1.0         | 50 gp3/3,000/125           |
| **t3.nano**     | `222,132` | `20`             |                  | x86_64 Intel | 2    | 0.5         | 50 gp3/3,000/125           |
| t3.micro        | `228,721` | `19`             | `0.0455`         |              | 2    | 1.0         |                            |
| t3.small        | `217,802` | `19`             |                  |              | 2    | 2.0         |                            |
| t3.meduim       | `222,703` | `20`             |                  |              | 2    | 4.0         |                            |
| t3.large        | `220,726` | `20`             |                  |              | 2    | 8.0         |                            |
| **t3a.nano**    | `138,240` | `12`             |                  | x86_64 AMD   | 2    | 0.5         |                            |
| t3a.micro       | `138,124` | `12`             |                  |              | 2    | 1.0         |                            |
| t3a.small       | `137,146` | `12`             |                  |              | 2    | 2.0         |                            |
| t3a.medium      | `136,304` | `12`             |                  |              | 2    | 4.0         |                            |
| t3a.large       | `137,981` | `12`             |                  |              | 2    | 8.0         |                            |
| **t4g.nano**    | `168,350` | `15`             |                  | arm64        |      |             |                            |
| t4g.micro       | `167,728` | `15`             |                  |              |      |             |                            |
| t4g.small       | `170,334` | `15`             |                  |              |      |             |                            |
| t4g.medium      | `170,847` | `15`             |                  |              |      |             |                            |
| t4g.large       | `168,118` | `15`             |                  |              |      |             |                            |
| **m6i.large**   | `323,132` | `30`             |                  | x86_64 Intel |      |             |                            |
| m6id.large      | `320,875` | `29`             |                  |              |      |             | 118 NVMe SSD / 16,771      |
| m6a.large       | `301,792` | `28`             |                  | x86_64 AMD   |      |             | 50 gp3/3,000/125           |
| m6g.medium      | `167,426` | `15`             |                  | arm64        |      |             |                            |
| m6g.large       | `166,895` | `15`             |                  |              |      |             |                            |
| m6gd.medium     | `170,734` | `15`             |                  |              |      |             | 59 NVMe SSD / 5,625        |
| m6gd.large      | `173,216` | `16`             |                  |              |      |             | 118 NVMe SSD / 11,250      |
| m7g.medium      |           |                  |                  |              |      |             | 50 gp3/3,000/125           |
| m7g.large       |           |                  |                  |              |      |             |                            |
| **c6i.large**   | `320,649` | `29`             |                  | x86_64 Intel |      |             |                            |
| c6id.large      | `320,041` | `29`             |                  |              |      |             | 118 NVMe SSD / 16,771      |
| c6a.large       | `300,907` | `28`             |                  | x86_64 AMD   |      |             | 50 gp3/3,000/125           |
| c6g.medium      | `170,883` | `15`             |                  | arm64        |      |             |                            |
| c6g.large       | `172,609` | `16`             |                  |              |      |             |                            |
| c6gd.medium     | `172,718` | `16`             |                  |              |      |             | 59 NVMe SSD / 5,625        |
| c6gd.large      | `172,260` | `16`             |                  |              |      |             | 118 NVMe SSD / 11,250      |
| c7g.medium      |           |                  |                  | arm64        |      |             | 50 gp3/3,000/125           |
| c7g.large       |           |                  |                  |              |      |             |                            |
| **r6i.large**   | `322,264` | `30`             |                  | x86_64 Intel |      |             |                            |
| r6id.large      | `320,875` | `29`             |                  |              |      |             | 118 NVMe SSD / 16,771      |
| r6a.large       | `303,916` | `28`             |                  | x86_64 AMD   |      |             | 50 gp3/3000/125            |
| r6g.medium      | `166,116` | `15`             |                  | arm64        |      |             |                            |
| r6g.large       | `170,337` | `15`             |                  |              |      |             |                            |
| r6gd.medium     | `165,836` | `15`             |                  |              |      |             | 59 NVMe SSD / 5,625        |
| r6gd.large      | `169,024` | `15`             |                  |              |      |             | 118 NVMe SSD / 11,250      |
| r7g.medium      |           |                  |                  |              |      |             | 50 gp3/3,000/125           |
| r7g.large       |           |                  |                  |              |      |             |                            |
| **i3.large**    |           |                  |                  | x86_64 Intel |      |             | 475 NVMe SSD / 35,000      |
| i4i.large       | `321,133` | `29`             |                  |              |      |             | 468 AWS Nitro SSD / 27,500 |

 - `50 gp3/3,000/125` - 50 GB EBS disk, GP3 with 3000 IOPS and 125 MB/s throughput
 - `59 NVMe SSD / 5,625` - 59 GB NVMe SSD disk with 5,625 write IOPS


## Conclusions

 1. Logfilegen is a single-thread application and we did not get any significant performance improvement on multi-cpu systems.
 2. Logfilegen performs better on x86_64 then on arm64 architecture.
 3. T3/T4 instances have a chiper price and performs wery hell from cost/performance point of view.
 4. Instances with the instance store performs much better than instances on EBS.
