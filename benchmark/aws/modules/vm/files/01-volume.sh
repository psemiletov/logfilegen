#! /bin/bash

# Variables
MOUNT_POINT="${mount_point}"
DATA_VOLUME_SIZE="${data_volume_size}"

# Identify OS
distrib_id=$(hostnamectl | awk -F ': ' '/Operating System/ {print $2}')
if [[ "$${distrib_id,,}" == *"ubuntu"* ]]; then
  os=ubuntu
elif [[ "$${distrib_id,,}" == *"amazon"* ]]; then
  os=amazon
else
  os=unknown
fi


# Wait for the data volume to be attached
[[ "$DATA_VOLUME_SIZE" == "null" ]] && WAIT=10 || WAIT=60
SECONDS=0
while (( SECONDS < WAIT )); do
  count=$(lsblk -r | grep disk -c)
  echo "`date` - Found $count volume - waiting - $SECONDS/$(($WAIT-$SECONDS))/$WAIT"
  [[ "$count" > 1 ]] && { echo "Data volume was attached"; break; }
  sleep 1
done


# Stop if data volume was not attached in time
[[ "$count" < 2 ]] && { echo "Volume was not attached in time"; exit 1; }


# Find data volume device
while read ebs; do
  part=$(lsblk -r | grep "$ebs.* part")
  [[ -z "$part" ]] && device="$ebs"
done <<< "$(lsblk -r | awk '/disk/ {print $1}')"

echo "Data device is $device"


# Check if data volume contain FS
fs=$(lsblk -f | awk '/'$device'/ {print $2}')
[[ -z "$fs" ]] && { echo "Device $device does not contain FS"; fs=false; } || echo "Device $device already contain FS"


# Create FS and add volume to fstab
cp /etc/fstab{,.bak}

case "$os" in
  ubuntu) fs_type=ext4
          label=$(sed -r 's/^.{1}//' <<< "$MOUNT_POINT")
          [[ "$fs" == "false" ]] && mkfs -t "$fs_type" /dev/"$device"
          e2label /dev/"$device" "$label"
          echo "LABEL=$label              $MOUNT_POINT    $fs_type   defaults,nofail                 0 2" >> /etc/fstab
          ;;

  amazon) [[ "$fs" == "false" ]] && mkfs -t xfs /dev/"$device"
          sleep 1
          uuid=$(lsblk -no UUID /dev/"$device")
          echo "UUID=$uuid     $MOUNT_POINT       xfs    defaults,nofail   1   1" >> /etc/fstab
          ;;

       *) echo "Unknown OS"
          ;;
esac


# Mount volume
mkdir -p "$MOUNT_POINT"
mount -a
