from locust import HttpUser, task, between

class WebServerUser(HttpUser):
    wait_time = between(0.5, 2)  # wait 0.5-2 seconds between requests

    @task(3)  # weight 3 - most common request
    def load_index(self):
        self.client.get("/")

    @task(2)  # weight 2
    def load_page2(self):
        self.client.get("/page2.html")

    @task(1)  # weight 1 - test 404 handling
    def load_nonexistent(self):
        with self.client.get("/nonexistent.html", catch_response=True) as response:
            if response.status_code == 404:
                response.success()