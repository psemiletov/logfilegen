# Node information
output "ssh_connection" {
  description = "SSH conection"
  value       = module.instance-01.ssh_connection
}

output "instance_id" {
  description = "Node instance ID"
  value       = module.instance-01.instance_id
}
