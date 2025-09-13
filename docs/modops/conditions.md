# ModOp Conditions

Conditions can be used to skip ModOps.

The operation will only be applied when the `Condition` matches.
Conditions can be used with all ModOps: `ModOp`, `Group`, `Include` and `Asset`.

## With ModID

You can use conditions to check if a mod exists.
This works in any patch - not only `assets.xml` patches.

It doesn't matter whether the mod is loaded before or after this mod.
Use `LoadAfterIds` to ensure it's loaded before your mod.

=== "117 & 1800"
    ```xml
    <Group Condition="#mod-id">
      <!-- Apply ModOps if `mod-id` is active. -->
    <Group>

    <Group Condition="!#mod-id">
      <!-- Apply ModOps if `mod-id` is NOT active. -->
    <Group>
    ```

New in Anno 117: These `#mod-id` expressions are now expanded to `true()` and `false()` to be used within XPath.
That means you can combine them like XPath expressions with `and` and `or`.

=== "117"
    ```xml
    <Group Condition="#mod-a and not(#mod-b)">
      <!-- ... -->
    </Group>
    ```
=== "117 & 1800"
    ```xml
    <Group Condition="#mod-a">
      <Group Condition="!#mod-b">
        <!-- ... -->
      </Group>
    </Group>
    ```


## With XPath

=== "117 & 1800"
    ```xml
    <Group Condition="//Values[Standard/GUID='1500010714']">
      <!-- Apply ModOps if asset 1500010714 exists. -->
    </Group>

    <Group Condition="!//Values[Standard/GUID='1500010714']">
      <!-- Apply ModOps if 1500010714 does NOT exist. -->
    </Group>
    ```

## With @GUID

Use `@` as a short version of `//Values[Standard/GUID='guid']`.
This works only at the start of an XPath.

=== "117 & 1800"
    ```xml
    <Group Condition="@1500010714">
      <!-- Apply ModOps if asset 1500010714 exists. -->
    </Group>

    <Group Condition="!@1500010714">
      <!-- Apply ModOps if 1500010714 does NOT exist. -->
    </Group>
    ```

## Relative Condition

Conditions can be relative to the `GUID` your ModOp is operating on.

=== "117"
    ```xml
    <ModOp GUID="800111"
      Condition="!Property/List[Item/Product='1500010120']"
      Add="Property/List">
      <Item>
        <Product>1500010120</Product>
      </Item>
    </ModOp>
    ```
=== "117 & 1800"
    ```xml
    <ModOp Type="add" GUID="800111"
      Condition="!/Values/Property/List[Item/Product='1500010120']"
      Path="/Values/Property/List">
      <Item>
        <Product>1500010120</Product>
      </Item>
    </ModOp>
    ```

## Skip

!!! info "You only need this feature for iModYourAnno tweaks."

The attribute `Skip` can be used with `Group` and `Include` to ignore that operation.

```xml
<ModOps>
  <Include File="/feature.include.xml" Skip="1" />
</ModOps>
```

Note: the skip happens when the attribute `Skip` is present.
It doesn't matter if you write `Skip="1"`, `Skip="True"` or even `Skip="0"` - all of them lead to skipping the include.

## Best Practice: Prefer Negative Conditions

The following two ModOps will lead to the same result, but have a stark difference.

=== "117 & 1800"
    ```xml
    <!-- negative condition -->
    <ModOp Type="add" GUID="800111"
      Condition="!~/Feature/Options[Item/Product='1500010120']"
      Path="/Values/Feature/Options/FeedOptions" />

    <!-- positive condition -->
    <ModOp Type="add" GUID="800111"
      Condition="~/Feature/Options[not(Item/Product='1500010120')]"
      Path="/Values/Feature/Options" />
    ```

If GUID `800111` is missing, or `Feature` has been removed from it, then the negative condition be true and the ModOp throws a warning because of a not found node.

On the other hand, the positive condition will skip the ModOp without any warning as the condition was not met.

## Disable warnings with `AllowNoMatch`

Disable match not found warnings.
Useful if you want to add items, but only if they don't exist.

!!! warning "Use AllowNoMatch sparingly as it hides all warnings"

It's better to use conditions in many cases, as conditions still allow some warnings when there are issues.

Adding a product only once to a list with `AllowNoMatch`:

=== "117 & 1800"
    ```xml
    <ModOp Type="add" GUID="120055"
          Path="/Values/List[not(Item/Product='1500010102')]"
          AllowNoMatch="1">
      <Item>
        <Product>1500010102</Product>
      </Item>
    </ModOp>
    ```
