# ModOp Type `replace` and `merge`

Content

- [Replace](#modop-type-replace)
  - [Replace to swap content](#replace-to-swap-content)
  - [Replace to remove content](#replace-to-remove-content)
  - [Replace to remove node](#replace-to-remove-node)
  - [Best practice: avoid large replace](#best-practice-avoid-large-replace)
- [Merge](#modop-type-merge)
  - [Merge is order indepenent](#merge-is-order-independent)
  - [Merge adds missing nodes](#merge-adds-missing-nodes)
  - [Merge does not remove nodes](#merge-does-not-remove-nodes)
  - [Merge does not support lists](#merge-does-not-support-lists)
  - [Merge can be used like `add` or `replace`](#merge-can-be-used-like-add-or-replace)

Other docs

- [Simple ModOps](./modop-guide.md)
- [Conditions](./modop-conditions.md)
- [Examples](./modop-examples.md)

## ModOp Type `replace`

### Replace to swap content

Type completely replaces the selected nodes with the content.

```xml
<ModOp GUID="123" Type="replace" Path="/Values/Standard">
  <Standard>
    <GUID>456</GUID>
    <Description>Hello</Description>
  </Standard>
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
-     <GUID>123</GUID>
-     <Name>some name</Name>
+     <GUID>456</GUID>
+     <Description>Hello</Description>
    </Standard>
    <Cost />
  </Values>
</Asset>
```

Note: the element to be replaced (`Standard` in this example) needs to be mentioned as well.

### Replace to remove content

```xml
<ModOp GUID="123" Type="replace" Path="/Values/Cost">
  <Cost />
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
     <GUID>123</GUID>
    </Standard>
    <Cost>
-     <Item />
    </Cost>
  </Values>
</Asset>
```

### Replace to remove node

An empty `replace` is essentially the same as `remove`.

Both ModOps have the same output:
```xml
<ModOp GUID="123" Type="replace" Path="/Values/Cost">
</ModOp>

<ModOp GUID="123" Type="remove" Path="/Values/Cost" />
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
     <GUID>123</GUID>
    </Standard>
-   <Cost>
-     <Item />
-   </Cost>
  </Values>
</Asset>
```

### Best Practice: avoid large replace

Imagine you want to do a change in a production chain.
It is better to insert, merge or do a very small replace instead of replacing the whole chain to not overwrite other mods doing little changes in the same location.

## ModOp Type `merge`

### Merge is order independent

The order of nodes do not matter anymore for successful merges.
Before you had to have the exact same order as the game.

```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <AllowChangeDirection>1</AllowChangeDirection>
  <AllowChangeVariation>1</AllowChangeVariation>
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <Building>
-     <AllowChangeVariation>0</AllowChangeVariation>
-     <AllowChangeDirection>0</AllowChangeDirection>
+     <AllowChangeVariation>1</AllowChangeVariation>
+     <AllowChangeDirection>1</AllowChangeDirection>
    </Building>
  </Values>
</Asset>
```

### Merge adds missing nodes

```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <AllowChangeVariation>1</AllowChangeVariation>
  <BuildModeStartVariation>0</BuildModeStartVariation>
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <Building>
-     <AllowChangeVariation>0</AllowChangeVariation>
+     <AllowChangeVariation>1</AllowChangeVariation>
+     <BuildModeStartVariation>0</BuildModeStartVariation>
    </Building>
  </Values>
</Asset>
```

### Merge does not remove nodes

```xml
<ModOp GUID="123" Type="merge" Path="/Values">
  <Building />
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <Building>
     <AllowChangeVariation>1</AllowChangeVariation>
    </Building>
  </Values>
</Asset>
```

Use `replace` or `remove` instead if you want to remove content.

### Merge does not support lists

Merging with multiple same name nodes (usually `Item`) is not supported anymore.

Relying on index is prone to compatibility issues.

```xml
<ModOp Type="merge" GUID="100780" Path="/Values/Maintenance">
  <Maintenances>
    <Item>
      <Product>1010017</Product>
      <Amount>50000</Amount>
      <InactiveAmount>30000</InactiveAmount>
    </Item>
    <Item>
      <Product>1010367</Product>
      <Amount>50</Amount>
    </Item>
  </Maintenances>
</ModOp>
```

Do individual merges instead:

```xml
<ModOp Type="merge" GUID="100780" Path="/Values/Maintenance/Maintenances/Item[Product='1010017']">
  <Amount>50000</Amount>
  <InactiveAmount>30000</InactiveAmount>
</ModOp>
```

```xml
<ModOp Type="merge" GUID="100780" Path="/Values/Maintenance/Maintenances/Item[Product='1010367']">
  <Amount>50</Amount>
</ModOp>
```

### Merge can be used like `add` or `replace`

The following to merge operations lead to the same result.

Used like add, will only add if not there already:
```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <AllowChangeVariation>1</AllowChangeVariation>
</ModOp>
```

Used like replace including the last path element (i.e. `Building`):
```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <Building>
    <AllowChangeVariation>1</AllowChangeVariation>
  </Building>
</ModOp>
```

You need to use the second approach in some rare cases when parent and child element have the same name - which happens in localizations for example (`Texts/Text/Text/GUID`).
It only applies if there is exactly one top-level element in the ModOp that has the same name as the last path element.

*Note the following changed behavior since modloader10 / GU17.*

The following ModOp doesn't update `AllowChangeVariation` anymore.
Yes, it did before...
It will instead add the node to `Values`.

```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <AllowChangeVariation>1</AllowChangeVariation>
  <Building>
    <BuildModeStartVariation>0</BuildModeStartVariation>
  </Building>
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <Building>
      <AllowChangeVariation>0</AllowChangeVariation>
+     <BuildModeStartVariation>0</BuildModeStartVariation>
+     <Building>
+       <AllowChangeVariation>1</AllowChangeVariation>
+     </Building>
    </Building>
  </Values>
</Asset>
```
