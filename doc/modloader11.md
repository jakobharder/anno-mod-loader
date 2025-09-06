# ModLoader 11 Changes

These are proposals and may or may not end up in the game.

- [Patching .cfg files](#patching-cfg-files)
- [Patching .fc files](#patching-fc-files)
- [Link files](#link-files)
- [Wrap ModOp Content](#wrap-modop-content)

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
          <Id>$auto</Id> <!-- use auto -->
          <HeightAdaptationMode>1</HeightAdaptationMode>
        </i>
      </Dummies>
      <Id>$auto</Id>
    </i>
    <!-- ... -->
  </ModOp>
</ModOps>
```

Note: you can use `$auto` to automatically assign free ids. Use `$auto(0)`, `$auto(1)`, ... if you want to use the same auto id multiple times.

The format is the same as used in the [FileDBReader](https://github.com/anno-mods/FileDBReader/blob/master/FileFormats/FcFile.xml).

## Link files

You can link files by adding a text file with the extension `.lnk`.
The path and name of the file is the link source, the text file contents is the link target.

This is useful to duplicate islands under a different path for example.

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