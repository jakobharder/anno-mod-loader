def define_env(env):
    @env.macro
    def v8():
        return f'<span class="version-tag" title="Only available in Anno 117">:material-pillar: 117</span>'

    @env.macro
    def v7():
        return f'<span class="version-tag" title="Only available in Anno 1800">:material-factory: 1800</span>'

    @env.macro
    def v0():
        return f'<span class="version-tag" title="Available in all supported Annos">:material-animation-outline: 117 & 1800</span>'