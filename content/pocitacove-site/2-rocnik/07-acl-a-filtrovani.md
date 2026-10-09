---
id: site-2-acl-a-filtrovani
puvodni: net2.7
nazev: ACL a filtrování
popis: Seznam, který provoz pustí a který zahodí.
nazev_cviceni: Kvíz: ACL a filtrování
nadpis_kvizu: Kvíz k tématu ACL a filtrování
---

# Výklad

## Pořadí rozhoduje
*Pravidla*

- ACL zkouší řádky shora dolů.
- Na konci bývá tiché zamítnutí všeho ostatního.
- Pravidlo umí adresu, protokol a port.
- Jedno špatné pořadí otevře službu, kterou jste chtěli zavřít.

> Tip: první shoda vyhrává. Obecné povolení na začátku pozdější zákaz schová.

## Dovnitř a ven
*Směr*

- Příchozí ACL sedí na paketech, které do rozhraní vcházejí.
- Odchozí ACL sedí na paketech, které rozhraní opouštějí.
- Filtr na VLAN rozhraní hlídá provoz mezi sítěmi.
- ACL na směrovači nevidí provoz, který zůstane uvnitř jednoho switche.

> Tip: blíž zdroji filtrujte to, co nemá vůbec vstoupit.

## Co obvykle pustit
*Praxe*

- Správa směrovače nemá poslouchat z hostovské VLAN.
- Do serverové sítě pusťte jen porty služeb, které tam jsou.
- ICMP můžete omezit, ale úplný zákaz ztíží hledání chyb.
- Po změně ACL ověřte, že legitimní provoz pořád prochází.

> Tip: nejdřív povolte, co potřebujete, a zbytek nechte spadnout.


# Kvíz

## Jak ACL vybírá pravidlo?
- [x] Použije první řádek, který sedí
- Sečte všechny řádky a udělá průměr
- Vždy použije poslední řádek
- Ptá se DNS
> Pořadí je součást významu seznamu.

## Co udělá tiché zamítnutí na konci?
- [x] Zahodí provoz, který žádný řádek nepovolil
- Povolí všechno, co zbylo
- Přepne port do trunku
- Pošle paket do IPv6
> Co není povoleno, neprojde.

## Proč ACL na routeru nehlídá dva počítače v jedné VLAN?
- [x] Jejich rámce jdou jen přes switch a k routeru nemusí
- ACL umí jen Wi-Fi
- VLAN nemá MAC adresy
- TCP takový provoz šifruje před switchem
> Provoz uvnitř VLAN zůstane na druhé vrstvě.

## Kam nepatří správa routeru?
- [x] Do hostovské sítě
- Na konzolový port technika
- Do správní VLAN
- Na šifrované SSH z dohledu
> Host nemá mít cestu na správu infrastruktury.
