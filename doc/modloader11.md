# ModLoader 11 Changes

These are proposals and may or may not end up in the game.

- [Patching InfoTip export.bin](#patching-infotip-exportbin)
- [Patching .cfg files](#patching-cfg-files)
- [Patching .fc files](#patching-fc-files)
- [Conditions with ModID](#conditions-with-modid)
- [Wrap ModOp Content](#wrap-modop-content)

## Patching InfoTip export.bin

You can patch `data/infotips/export.bin` by adding an `export.bin.xml` patch file to your mod.

Example:
```xml
<ModOps>
  <ModOp Type="merge" Path="//InfoTipData[Guid='500934']">
    <InfoElement>
      <VisibilityElement>
        <ChildCount>2</ChildCount>
        <VisibilityElement />
        <VisibilityElement>
          <ElementType>
            <ElementType>1</ElementType>
          </ElementType>
          <CompareOperator>
            <CompareOperator>1</CompareOperator>
          </CompareOperator>
          <ResultType>
            <ResultType>1</ResultType>
          </ResultType>
          <ExpectedValueInt>102284</ExpectedValueInt>
          <Condition>[Selection Object GUID]</Condition>
        </VisibilityElement>
      </VisibilityElement>
    </InfoElement>
    <!-- ... -->
  </ModOp>
</ModOps>

```

Note: the global `InfoTipCount` and `TemplateCount` will be adjusted automatically, but you need to ensure `ChildCount`s are correct yourself.

The format is the same as used in the [FileDBReader](https://github.com/anno-mods/FileDBReader/blob/master/FileFormats/infotip.xml).

## Patching .cfg files

You can patch .cfg files e.g. `world_map_01.cfg` by adding an XML file with the same name plus `.xml` as an extension.

```xml
<ModOps>
  <ModOp Type="add" Path="//Models">
    <Config>
      <ConfigType>MODEL</ConfigType>
      <!-- ... -->
    </Config>
  </ModOp>
</ModOps>
```

Note: patching is more expensive than replacing. Avoid it if you can.

## Patching .fc files

You can patch .fc files e.g. `world_map_01.fc` by adding an XML file with the same name plus `.xml` as an extension.

Example:
```xml
<ModOps>
  <ModOp Type="add" Path="/DummyRoot/Groups">
    <i>
      <hasValue>1</hasValue>
      <Name>2111001</Name>
      <Groups />
      <Dummies>
        <i>
          <hasValue>1</hasValue>
          <Name>2111001_0</Name>
          <!-- position -->
          <Id>auto</Id> <!-- use auto -->
          <HeightAdaptationMode>1</HeightAdaptationMode>
        </i>
      </Dummies>
      <Id>auto</Id>
    </i>
    <!-- ... -->
  </ModOp>
</ModOps>
```

Note: you can use `<Id>auto</Id>` to automatically assign free ids.

The format is the same as used in the [AnnoFCConverter](https://github.com/jakobharder/AnnoFCConverter).

## Link files

You can link files by adding a text file with the extension `.lnk`.
The path and name of the file is the link source, the text file contents is the link target.

This is useful to duplicate islands under a different path for example.

## Conditions with ModID

You can use conditions to check if a mod exists in any patch - not only `assets.xml` patches.

It doesn't matter whether the mod is loaded before or after this mod.
Use `LoadAfterIds` to ensure it's loaded before your mod.

```xml
<Group Condition="#NewWorldDocklands">
  <ModOp Type="add" Path="@1404422070/ModuleOwner/ConstructionOptions">
    <Item>
      <ModuleGUID>1500010080</ModuleGUID>
    </Item>
  </ModOp>
<Group>
```

## Wrap ModOp `Content`

```xml
<ModOp Type="merge" Path="@1010399/AssetPool/AssetList"
  Content="@120055/ProductStorageList/ProductList/Item/Product/text()">
  <Item>
    <Asset>
      <ModOpContent />
    </Asset>
  </Item>
</ModOp>
```

Content source:

```xml
<ProductList>
  <Item>
    <Product>120008</Product>
  </Item>
  <!-- ... -->
</ProductList>
```

Merge target:

```xml
<AssetPool>
  <AssetList>
    <Item>
      <Asset>120008</Asset>
    </Item>
    <!-- ... -->
  </AssetList>
</AssetPool>
```