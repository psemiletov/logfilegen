# AWS EC2 Instance

 1. [Description](#description)
 2. [Requirements](#requirements)
 3. [Deploy](#deploy)
 4. [Automated benchmarking](#automated-benchmarking)
 5. [Known issues](#known-issues)


## Description

 During development, there maybe the cases when we need to perform some test or just to check how application is working in a commonly used environment like Cloud Providers.

 This configuration provides the code which can run [EC2 instances](https://aws.amazon.com/ec2/) on [Amazon Web Services](https://aws.amazon.com).
 We may consider to use it in the following modes
 1. [Daily mode](#deploy)
 2. [Automated benchmarking](#automated-benchmarking)


## Requirements

 In order to proceed with this code usage, we need
 1. Linux host with [Terraform](https://developer.hashicorp.com/terraform/tutorials/aws-get-started/install-cli) installed.
 2. AWS user account with the following programmatic access permissions
      - `AmazonVPCFullAccess`
      - `AmazonEC2FullAccess`
      - `AmazonS3FullAccess`
      - `IAMFullAccess`


## Deploy

 1. [Install Terraform](https://developer.hashicorp.com/terraform/tutorials/aws-get-started/install-cli) and we also may consider to use [tfenv](https://github.com/tfutils/tfenv) for managing different versions

 2. Get the code
    ```bash
    git clone https://github.com/psemiletov/logfilegen.git
    cd benchmark/aws
    ```

 3. Create configuration
    ```bash
    vi personal.auto.tfvars
    ```
    ```javascript
    region        = "eu-west-1"
    name          = "logfilegen"
    instance_type = "t3.micro"
    key_name      = "aws key name"
    # public_key  = "ssh-rsa ..."
    ```
    For more details about configuration, please see [Configuration](#configuration).

 4. Authenticate on AWS
    ```bash
    export AWS_ACCESS_KEY_ID="access key"
    export AWS_SECRET_ACCESS_KEY="secret access key"
    ```

 5. Run Terraform
    ```bash
    # Initialize
    terraform init

    # View execution plan
    terraform plan

    # Apply changes
    terraform apply
    ```

 6. In for bout 1 minute instance will be created and `user@ip` for SSH connection will be shown in the output

 7. After the work was finished we should cleanup created resources
    ```bash
    # Destroy resources
    terraform destroy

    # Optional
    rm -rf .terraform* terraform.tfstate*
    ```


### Configuration

#### **General**

 Provided Terraform configuration works in the following way
 1. `aws` - Root module folder
 2. `aws/modules/vm` - Child module folder
 3. Configuration is defined in the Root module `main.tf` file and it calls Child module with the parameters

 Child module has default values which can be overridden from the Root module. Configuration parammeters are applied in the following order
   - Child module default values in `variables.tf`
   - Root module default values in `variables.tf`
   - Environment variables `TF_VAR_<var>=<value>`
   - Root module values in `*.auto.tfvars`
   - Variables passed to `plan`/`apply` from the CLI using `-var="<var>=<value>"`

 For more information, plese see [Input Variables](https://developer.hashicorp.com/terraform/language/values/variables).


#### **Multiple instances**

 We may run multiple instances by adding them to the Root module `main.tf` file and they just need to have an unique module name
```
module "instance-02" {
  source = "./modules/vm"

  name                  = "logfilegen-2"
  os_name               = "amazon"
  instance_architecture = "arm64"
  instance_type         = "t4g.micro"
  key_name              = var.key_name
  allow_tcp_ports       = [8080]
  logfilegen_version    = "2.0.0"
}
```

 In this example we use a common value for `key_name` defined in the variables and the rest of values are passed directly.

 If we also would like to see the output about other instances, we should add appropriate records to the Root module `outputs.tf` file.


#### **Configuration variables**

#### **VPC**

 To simplify and speed up instance provisioning, we use just a default VPC in the specified region

`subnet_id` - default is `null` and can be
 - `null` - a random subnet will be used from a default VPC in the current region
 - `subnet_id` - specify a subnet id from a default VPC

 We also may specify which Availability Zone to use by set `az` variable, for example `az = "c"`.


#### **Security Group**

`vpc_security_group_ids` - default is `[]` and can be
 - `[]`     - in case when `allow_ssh = true` or `allow_tcp_ports` is set, a new security group will be created, othervise a default security group will be used
 - `[list]` - we can specify list of security groups to be attached to the instance

 By default, variable `allow_ssh = true` and it means that a new Security Group will be created with the rule to allow SSH access from any IPs.

 In case we need to open more TCP ports we should define them as follow `allow_tcp_ports = [5601, 8080]`


#### **OS**

 Default used OS is ubuntu and we also may set `os_name = amazon`. More OS can be added in the `aws-ami.tf` file of the Child module and we also should update [user_data](#user-data) scripts.
```
ubuntu-16 = ubuntu 16.04
ubuntu-18 = ubuntu 18.04
ubuntu-20 = ubuntu 20.04
ubuntu-22 = ubuntu 22.04
ubuntu    = ubuntu 22.04
amazon    = amazon linux 2
```

#### **Instance type**

 We may speciify instance type in the `instance_type` variable, please see [Instance types](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/instance-types.html) for available options.

 In AWS there is a relation between instance type and instance architecture which is related to the [AMI](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/AMIs.html) and it means that, when we set for example `instance_type = "t4g.micro"`, we also should set `instance_architecture = "arm64"`.


#### **SSH Key**

 We have at least 3 ways to connect to the instance
 1. `key_name` - specify an existing key pair on AWS EC2
 2. `public_key` - specify a self-generated public RSA key
 3. When nothing is specified, instance will be created without the key and a single option to connect to it will be to use [Connect using EC2 Instance Connect](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/ec2-instance-connect-methods.html).


#### **Disk**

 We can set disk type for **root** and **data** volumes of the instance, using following variables
```
  root_volume_size       = 8
  root_volume_type       = "gp3"
  root_volume_iops       = 3000
  root_volume_throughput = 125

  data_volume_size       = 50
  data_volume_type       = "gp3"
  data_volume_iops       = 16000
  data_volume_throughput = 1000
```

 For more information please see [Amazon EBS volume types](https://aws.amazon.com/ebs/volume-types/).

 When we set `data_volume_size` bigger than `0`, a separate data volume will be created and attached to the instance and [user_data](#user-data) script will take care to format and mount it to the OS.

 Also, please note that for the instances with the [Amazon EC2 instance store](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/InstanceStorage.html) we may consider to not add data volume and instead use an instance store one.
 To disable EBS based volume creation we just need to set `data_volume_size = 0` or just not specify it and the same default value will be applied.


#### **Shutdown**

 By default `shutdown_behavior = true` and it means that the instance will be terminated (deleted) once we will shutdown it, for example using `halt -p` on Linux. But we should be aware that if we have attached EBS based data volume, it will remain on AWS and we need to delete it manually from AWS Console or just using `terraform destroy`.


#### **User data**

 On AWS we may [Run commands on your Linux instance at launch](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/user-data.html).

 In the current configuration we are doing this using shell scripts located in the Child module [files](modules/vm/files) directory and they are copied by the code in the [`aws-ec2.tf`](modules/vm/aws-ec2.tf) file. At the copying we add number to them to control execution order.
```
enable_volume        # 01-volume.sh
enable_node          # 02-node.sh
enable_logfilegen    # 03-logfilegen.sh
enable_benchmark     # 04-benchmark.sh
enable_docker        # 05-docker.sh
```

 All the scripts controlled by the appropriate variables in order to be copied to the instance and executed.

 And some variables used just to perform automated benchmarking
```
  benchmark_count    = 3
  benchmark_duration = 300
  benchmark_auto     = false
  results_bucket     = null
  results_bucket_key = benchmark
```


## Automated benchmarking

 We can perform an automated benchmarking using [GitHub Actions](https://github.com/features/actions) and for this we have a [Benchmark workflow](../../.github/workflows/benchmark.yaml).

 It works in the following way
 1. We run GitHub Actions workflow and pass input variables
 2. Input variables passed to the Terraform
 3. Terraform creates
      - AWS IAM Role for EC2 Instance to be abble to acess AWS S3 bucket to upload benchmarking results
      - Run EC2 instance in the defaults VPC
      - Pass cloud-init data to bootstrap the instance
 4. Cloud-init script will do
      - Mount data volume
      - Install additional packages
      - Install logfilegen
      - Wait 1 minute to finish all post-run tasks
      - Run benchmark for specified count and duration
      - Save results to AWS S3
      - Shutdown the instance
 5. EC2 instance will be terminated on shutdown
 6. GitHub Actions will wait for instance to be in a terminated state
 7. Terraform will destroy all created resources
 8. All the steps will be applied in a loop using [GitHub Actions Matrix](https://docs.github.com/en/actions/examples/using-concurrency-expressions-and-a-test-matrix) to the list of specified instances types
 9. After all specified instances in the current run will be tested a report will be generated using the data stored on AWS S3


### Benchmarking

 To run automated benchmarking we should perform the following steps

 1. Create AWS S3 bucket

    AWS --> S3 --> Create bucket
      - Bucket name: `logfilegen-benchmark`
      - AWS Region: `eu-west-1`

    --> Create bucket

    > **Note:** AWS S3 bucket name should be a global unique and we provide name just as an example

    > **Note:** We may consider to create S3 bucket in the same region where benchmarking will be performed



 2. Create AWS IAM user

    AWS --> IAM --> Users --> Add users
      - User name: `logfilegen-github`

    --> Next --> Attach policies directly
     - `AmazonVPCFullAccess`
     - `AmazonEC2FullAccess`
     - `AmazonS3FullAccess`
     - `IAMFullAccess`

    --> Next --> Create user

    AWS --> IAM --> Users --> `logfilegen-github` --> Security credentials --> Access keys --> Create access key --> Application running outside AWS --> Next
      - Description tag value: `GitHub Actions - Benchmark`

    **Save credentials in a safe place for future use**

    --> Done

 3. Add GitHub Actions Secrets

    GitHub --> logfilegen repository --> Settings --> Secrets and variables --> Actions --> New repository secret
    | Secret name             | Secret value           | Description                                         |
    | ----------------------- | ---------------------- | --------------------------------------------------- |
    | `AWS_ACCESS_KEY_ID`     | `<access key>`         | IAM User logfilegen-benchmark                       |
    | `AWS_SECRET_ACCESS_KEY` | `<secret access key>`  | IAM User logfilegen-benchmark                       |
    | `AWS_REGION`            | `eu-west-1`            | Region where to run insatnces                       |
    | `RESULTS_BUCKET`        | `logfilegen-benchmark` | S3 bucket to store Terraform state and test results |
    | `RESULTS_BUCKET_KEY`    | `benchmark`            | S3 bucket folder to store benchmark results         |
    | `RESULTS_BUCKET_REGION` | `eu-west-1`            | S3 bucket region                                    |

    --> Add secret

 4. Run workflow

    > **Note:** Please consider to read [Known issues](#known-issues) before the run

    GitHub --> logfilegen --> Actions --> Benchmark --> Run workflow
      - Set required paramaters
        - For arm instances set *Instance architecture*: `arm64`
        - For instance store instances set all inputs for *Data volume*: `null`

    --> Run workflow
  

 5. After workflow will be finished we can find results in the output of the `Report` job.


### Cleanup

#### S3 content

 After we finished all the tests we may consider to delete manually S3 bucket or just its content to not incur any costs.


#### EC2 resources

 In some cases GitHub Actions may fail during the execution and if resources will be already created we may remove them using one of the following way

 1. Run GitHub Action [Benchmark workflow](../../../../actions/workflows/benchmark.yaml), select failed instances type and check `Just cleanup failed resources`

 2. Run GitHub Action [Benchmark workflow](../../../../actions/workflows/benchmark.yaml) again and Terraform will create just missed resources and after benchmarking will cleanup everything.

 3. Manually from AWS Console
    - EC2 Instance
    - EC2 Volume
    - IAM Role

 4. Using Terraform locally
    ```bash
    # Authenticate
    export AWS_ACCESS_KEY_ID="access key"
    export AWS_SECRET_ACCESS_KEY="secret access key"

    # Define variables
    instance_type="t3.micro"
    bucket="logfilegen-benchmark"
    region="eu-west-1"

    # Activate remote backend
    mv backend.tf_ backend.tf

    # Initialise backend
    terraform init \
      -backend-config="bucket=$bucket" \
      -backend-config="key=terraform/$instance_type" \
      -backend-config="region=$region"

    # List resources in the state
    terraform state list

    # Destroy resources
    terraform destroy
    ```


## Known issues

 1. [GitHub Actions workflow_dispatch inputs is limited to 10](https://github.com/community/community/discussions/8774) and some variables moved to the workflow code.

 2. We should not run more than one test for the same instance type simultaneously, because some unique resources like IAM role, Terraform state file will overlap.

 3. We should adjust `max_parallel` value, at [automated benchmarking](#automated-benchmarking) run, according to the [Amazon EC2 service quotas](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/ec2-resource-limits.html), otherwise we may get an issue with the instances creation.

 4. If we decide to create data volume or increase its size after instance was created we should handle that manually - [Make an Amazon EBS volume available for use on Linux](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/ebs-using-volumes.html).

 5. [User data script](#user-data) responsible for volumes mouting is adopted to handle just one additional volume mounting in to the `/data` directory. For the instances with the instance store it will mount local instance store volume instead of the EBS one. It means that, during [automated benchmarking](#automated-benchmarking) we may not be abble to check performance using EBS volume. With all that we may adopt it to handle multiple volumes with mount points like `/data`, `/data2` and so on, an additonal logic will be required to handle that on benchmarking side.

 6. For instances with instance store, we should ignore data in the reports about disk type and size.

 7. Some type of instances may not be accessible in a specific region, for example recently announced [M7g and R7g Instances](https://aws.amazon.com/about-aws/whats-new/2023/02/amazon-ec2-m7g-r7g-instances/).
