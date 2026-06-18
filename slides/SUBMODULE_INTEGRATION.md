# reveal.js Submodule + verisure.css Integration Proposal

## Current State

- `slides/reveal.js/` is now a git submodule pointing to `hakimel/reveal.js` (v6.0.1)
- `slides/verisure.css` is a standalone CSS file using reveal.js `--r-*` custom properties
- `talk.html` references `./reveal.js/dist/theme/verisure.css` (inside the submodule dist/)
- `talk.org` uses `#+REVEAL_THEME: verisure` (org-re-reveal convention)

## Integration Approach

### Option A: External theme file (recommended)

Keep `verisure.css` outside the submodule and reference it via a separate `<link>` tag:

```html
<link rel="stylesheet" href="./reveal.js/dist/reveal.css"/>
<link rel="stylesheet" href="./verisure.css" id="theme"/>
```

In org-re-reveal, override the theme path:

```org
#+REVEAL_THEME: none
#+REVEAL_EXTRA_CSS: ./verisure.css
```

Or keep the current `#+REVEAL_THEME: verisure` approach and symlink:

```bash
# Not needed if talk.html references ./verisure.css directly
ln -s ../../verisure.css slides/reveal.js/dist/theme/verisure.css
```

**Pros:**
- No modifications to the submodule
- Theme versioned alongside the presentation, not the framework
- Clean separation: framework updates don't touch the theme

**Cons:**
- org-re-reveal's `#+REVEAL_THEME:` expects themes inside `reveal.js/dist/theme/`
- May need a symlink or org config workaround

### Option B: Copy into submodule dist/ at build time

Add a make/script step that copies `verisure.css` into the submodule:

```bash
cp slides/verisure.css slides/reveal.js/dist/theme/verisure.css
```

**Pros:**
- Zero changes to talk.org or talk.html
- Works with org-re-reveal's theme resolution out of the box

**Cons:**
- Submodule directory becomes dirty (gitignored or post-checkout hook needed)
- Extra build step

### Recommendation

**Option B** is the pragmatic choice. Add a `.gitignore` entry in the submodule
or a top-level script, and the existing org/html files work unchanged.

Implementation:
1. Add a one-line script or Makefile target: `cp verisure.css reveal.js/dist/theme/`
2. The talk.html `<link>` already points to `./reveal.js/dist/theme/verisure.css`
3. Document the copy step in README or a slides/Makefile

## Theme Compatibility with reveal.js 6.0.1

The current `verisure.css` uses `--r-*` CSS custom properties which are the standard
theming API since reveal.js 4.x. It is **fully compatible** with 6.0.1.

Key differences in 6.0.1 vs the old hardcoded version:
- SCSS theme sources now use `@use 'sass:color'` instead of deprecated `darken()`/`lighten()`
- Template structure: `settings.scss` + `mixins.scss` + `theme.scss`
- Our CSS-only theme bypasses SCSS entirely, so these changes don't affect us
