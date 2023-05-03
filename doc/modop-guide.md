# ModOp Guide

Content

- [add](#type-add)
- [addNextSibling](#type-addnextsibling)
- [addPrevSibling](#type-addprevsibling)
- [remove](#type-remove)

Other docs

- [Replace and Merge](./modop-replace-merge.md)
- [Conditions](./modop-conditions.md)
- [Examples](./modop-examples.md)

## Simple ModOps

### Type `add`

Adds a new element at the end.

```xml
<ModOp GUID="123" Type="add" Path="/Values">
  <Maintenance />
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <Cost />
+   <Maintenance />
  </Values>
</Asset>
```

### Type `addNextSibling`

Adds an element after the selected one.

```xml
<ModOp GUID="123" Type="addNextSibling" Path="/Values/Standard">
  <Maintenance />
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
+   <Maintenance />
    <Cost />
  </Values>
</Asset>
```

A common situation is when you want to insert an item at a specific position.

```xml
<ModOp GUID="123" Type="addNextSibling" Path="/Values/ConstructionCategory/BuildingList/Item[Building='1000178']">
  <Item>
    <Building>123</Building>
  </Item>
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <ConstructionCategory>
      <BuildingList>
        <Item>
          <Building>1000178</Building>
        </Item>
+       <Item>
+         <Building>123</Building>
+       </Item>
        <Item>
          <Building>1010372</Building>
        </Item>
        <Item>
          <Building>1010343</Building>
        </Item>
      </BuildingList>
    </ConstructionCategory>
  </Values>
</Asset>
```

### Type `addPrevSibling`

Same as `addNextSibling` just before and not after.

### Type `remove`

Remove selected nodes.

```xml
<ModOp GUID="123" Type="remove" Path="/Values/Cost" />
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
     <GUID>123</GUID>
    </Standard>
-   <Cost />
  </Values>
</Asset>
```

