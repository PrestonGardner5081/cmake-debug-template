import sys
sys.path.append('/workspace/test_apps/rest_api/build')
from common_endpoint_api import CommonEndpointApi

api = CommonEndpointApi("https://158.101.127.192:8443/simple_storage", "https://trident.sensorops.io/api/auth/token")
api.authenticateWithUserPassword("preston", "openwater")
