DB diagram

```mermaid
erDiagram
    PillSlot {
        uuid id PK
        text pillName
        int pillCount
    }

    PillSchedule {
        uuid id PK
        uuid pillSlotId FK
        uuid profileId FK
        time[] dosageTimes
    }

    Owner {
        uuid id PK
        text username
        text passwordHash
        text salt
    }

    Profile {
        uuid id PK
        text username
    }

    RfidTag {
        uuid id PK
        uuid profileId FK
        timestamp createdAt
        timestamp lastUsed
        text tagId
    }

    ProcessedEvent {
        uuid id PK
        uuid profileId FK
        timestamp createdAt
        text eventType
        text eventData
    }

    PillSlot ||--o{ PillSchedule : has
    Profile ||--o{ PillSchedule : has
    Profile ||--o| RfidTag : has
    Profile ||--o{ ProcessedEvent : "associated with"
    Owner ||--o{ Profile : manages
```
