
Tutorial video
https://www.youtube.com/watch?v=Ro0rgeLDkO4

---

Go to API gateway console
=> custom domain name
=> create
    domain name: enter "api.stay-qool.com"
    TLS 1.2
    Edge optimise
    select you API

---

Again, Go to API gateway console
=> custom domain name
=> select the custom domain you created
=> API mapping
    => select your API and stage

---

Go to Route53 console
Hosted Zone
    => select your domain name
Create record
    => simple record
    => add "api."
    => select "A - Route ... AWS resource"
    => selecte "Alias to API Gateway API"
    => crete record

---

That's it !