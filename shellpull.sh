if [ ! -d /root/.ssh ]; then
mkdir -p /root/.ssh
chmod 700 /root/.ssh
fi
# Fetch public key using HTTP
ATTEMPTS=30
FAILED=0
while [ ! -f /root/.ssh/authorized_keys ]; do
curl -f http://169.254.169.254/latest/meta-data/public-keys/0/openssh-key > /tmp/metadata-key 2>/dev/null
if [ $? -eq 0 ]; then
cat /tmp/metadata-key >> /root/.ssh/authorized_keys
chmod 0600 /root/.ssh/authorized_keys
restorecon /root/.ssh/authorized_keys
rm -f /tmp/metadata-key
echo "Successfully retrieved public key from instance metadata"
echo "*****************"
echo "AUTHORIZED KEYS"
echo "*****************"
cat /root/.ssh/authorized_keys
echo "*****************"
else
FAILED=`expr $FAILED + 1`
if [ $FAILED -ge $ATTEMPTS ]; then
echo "Failed to retrieve public key from instance metadata after $FAILED attempts, quitting"
break
fi
echo "Could not retrieve public key from instance metadata (attempt #$FAILED/$ATTEMPTS), retrying in 5 seconds..."
sleep 5
fi
done
