# Node information
output "ssh_connection" {
  description = "SSH conection"
  value       = try("${local.username}@${aws_instance.this[0].public_ip}", null)
}

output "instance_id" {
  description = "Node instance ID"
  value       = try(aws_instance.this[0].id, null)
}
