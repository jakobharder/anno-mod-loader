# Patch CFG and FC Files

## Patching .cfg files

{{a1800}}

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

{{a1800}}

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